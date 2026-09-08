cmake_minimum_required(VERSION 3.28)

# ============================================================
# Project
# ============================================================

set(GAME_VERSION "0.0.0" CACHE STRING "Project version")

project(Jelly-Mayhem VERSION ${GAME_VERSION} LANGUAGES CXX)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/version.hpp"
    @ONLY
)


# ============================================================
# Project configuration
# ============================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Executables are placed in:
# build/<preset>/bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)


# ============================================================
# Dependencies
# ============================================================

include(FetchContent)

# Project does not use SFML Network.
# Disable it to avoid unnecessary dependencies such as MbedTLS.
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_Declare(
    CURL
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-8_16_0
)

set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

set(CURL_BUILD_EVERYTHING OFF CACHE BOOL "" FORCE)

set(USE_NGHTTP2 OFF CACHE BOOL "" FORCE)
set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

if(WIN32)
    set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
else()
    set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(SFML json CURL)

# ============================================================
# Source files
# ============================================================

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)


file(GLOB_RECURSE UPDATER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/updater/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/updater/*.hpp"
)


# ============================================================
# Executable
# ============================================================

if(WIN32)

    add_executable(jelly-mayhem WIN32
        ${SOURCES}
        ${HEADERS}
    )

    add_executable(jelly-mayhem-updater #TODO: add WIN32 after debugging
        ${UPDATER_SOURCES}
    )

else()

    add_executable(jelly-mayhem
        ${SOURCES}
        ${HEADERS}
    )

endif()

if(WIN32)
    target_sources(jelly-mayhem
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/jelly-mayhem.rc"
    )
endif()


target_include_directories(jelly-mayhem
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_link_libraries(jelly-mayhem
    PRIVATE
        SFML::Graphics
        SFML::Audio
)

target_compile_options(jelly-mayhem PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wreorder
)

target_include_directories(jelly-mayhem-updater
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/updater"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(jelly-mayhem-updater
    PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
)


# ============================================================
# Development
# ============================================================

# Copy assets next to the executable after every build.
#
# This is intended for local development.
# Release packages use install() instead.

add_custom_command(TARGET jelly-mayhem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:jelly-mayhem>/assets"
)


# ============================================================
# Installation
# ============================================================

include(GNUInstallDirs)


# ------------------------------------------------------------
# Runtime dependency search paths
# ------------------------------------------------------------

if(MINGW AND CMAKE_SYSTEM_NAME STREQUAL "Windows")

    execute_process(
        COMMAND
            "${CMAKE_CXX_COMPILER}"
            -print-search-dirs
        OUTPUT_VARIABLE
            PROJECT_GCC_SEARCH_DIRS
        ERROR_VARIABLE
            PROJECT_GCC_SEARCH_ERROR
        RESULT_VARIABLE
            PROJECT_GCC_SEARCH_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT PROJECT_GCC_SEARCH_RESULT EQUAL 0)
        message(FATAL_ERROR
            "Could not determine MinGW runtime library paths.\n"
            "Compiler: ${CMAKE_CXX_COMPILER}\n\n"
            "${PROJECT_GCC_SEARCH_ERROR}"
        )
    endif()

    string(
        REGEX MATCH
        "libraries:[^\n]*"
        PROJECT_GCC_LIBRARIES_LINE
        "${PROJECT_GCC_SEARCH_DIRS}"
    )

    if(NOT PROJECT_GCC_LIBRARIES_LINE)
        message(FATAL_ERROR
            "Could not determine GCC library search paths."
        )
    endif()

    string(
        REGEX REPLACE
        "^libraries:[ \t]*=[ \t]*"
        ""
        PROJECT_GCC_LIBRARY_PATHS
        "${PROJECT_GCC_LIBRARIES_LINE}"
    )

    string(
        REPLACE
        ":"
        ";"
        PROJECT_GCC_LIBRARY_PATHS
        "${PROJECT_GCC_LIBRARY_PATHS}"
    )

    set(PROJECT_RUNTIME_DIRECTORIES)

    foreach(PROJECT_PATH IN LISTS PROJECT_GCC_LIBRARY_PATHS)

        if(IS_DIRECTORY "${PROJECT_PATH}")

            list(
                APPEND
                PROJECT_RUNTIME_DIRECTORIES
                "${PROJECT_PATH}"
            )

        endif()

    endforeach()

    list(
        REMOVE_DUPLICATES
        PROJECT_RUNTIME_DIRECTORIES
    )

    message(STATUS "MinGW runtime search directories:")

    foreach(PROJECT_PATH IN LISTS PROJECT_RUNTIME_DIRECTORIES)

        message(STATUS "  ${PROJECT_PATH}")

    endforeach()


    # --------------------------------------------------------
    # CMake runtime dependency analyzer
    # --------------------------------------------------------

    set(
        CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM
        windows+pe
    )

    set(
        CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL
        objdump
    )

    set(
        CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND
        "${CMAKE_OBJDUMP}"
    )

endif()


# ------------------------------------------------------------
# Install executable and collect runtime dependencies
# ------------------------------------------------------------

message(STATUS "CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES:")
foreach(DIR IN LISTS CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES)
    message(STATUS "  ${DIR}")
endforeach()

message(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_OBJDUMP = ${CMAKE_OBJDUMP}")

if(MINGW AND CMAKE_SYSTEM_NAME STREQUAL "Windows")

    install(
        TARGETS jelly-mayhem jelly-mayhem-updater
        RUNTIME_DEPENDENCY_SET ProjectRuntime
        RUNTIME DESTINATION "."
    )

else()

    install(
        TARGETS jelly-mayhem
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endif()


# ------------------------------------------------------------
# Install Windows runtime dependencies
# ------------------------------------------------------------

if(MINGW AND CMAKE_SYSTEM_NAME STREQUAL "Windows")

    install(
        RUNTIME_DEPENDENCY_SET ProjectRuntime

        DESTINATION
            "."

        DIRECTORIES
            ${PROJECT_RUNTIME_DIRECTORIES}

        PRE_EXCLUDE_REGEXES
            "^(advapi32|bcrypt|crypt32|gdi32|iphlpapi|kernel32|msvcrt|opengl32|secur32|user32|wldap32|winmm|ws2_32)\\.dll$"
            "^api-ms-win-.*"
            "^ext-ms-win-.*"
    )

endif()


# ------------------------------------------------------------
# Install assets
# ------------------------------------------------------------

if(WIN32)

    install(
        DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
        DESTINATION
            "assets"
    )

else()

    install(
        DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
        DESTINATION
            "${CMAKE_INSTALL_DATADIR}/jelly-mayhem/assets"
    )

endif()

# start entries and icons

if(UNIX AND NOT APPLE)
    install(
        FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/packaging/linux/jelly-mayhem.desktop"
        DESTINATION
            "${CMAKE_INSTALL_DATADIR}/applications"
    )

    install(
        FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.png"
        DESTINATION
            "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps"
        RENAME
            "jelly-mayhem.png"
    )
endif()


# ============================================================
# Packaging
# ============================================================

set(CPACK_PACKAGE_NAME
    "Jelly-Mayhem"
)

set(CPACK_PACKAGE_VERSION
    "${PROJECT_VERSION}"
)

set(CPACK_NSIS_MENU_LINKS
    "jelly-mayhem.exe"
    "Jelly-Mayhem"
)

set(CPACK_PACKAGE_VENDOR
    "fsliwa"
)

set(CPACK_PACKAGE_CONTACT
    "fsliwa"
)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER
    "fsliwa"
)

set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "Jelly-Mayhem - a fast-paced 2D action game"
)

set(CPACK_NSIS_DISPLAY_NAME
    "Jelly-Mayhem"
)

set(CPACK_NSIS_PACKAGE_NAME
    "Jelly-Mayhem"
)

set(CPACK_NSIS_INSTALL_ROOT
    "$PROGRAMFILES64"
)

set(CPACK_PACKAGE_INSTALL_DIRECTORY
    "Jelly-Mayhem"
)

set(CPACK_NSIS_HELP_LINK
    "https://github.com/fsliwa/jelly-mayhem"
)

set(CPACK_NSIS_URL_INFO_ABOUT
    "https://github.com/fsliwa/jelly-mayhem"
)

set(CPACK_NSIS_CONTACT
    "fsliwa"
)

include(CPack)