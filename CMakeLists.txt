cmake_minimum_required(VERSION 3.28)

# ============================================================
# Project
# ============================================================

set(PROJECT1_VERSION "1.0.0" CACHE STRING "Project version")

project(Project1
    VERSION ${PROJECT1_VERSION}
    LANGUAGES CXX
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

# Project1 does not use SFML Network.
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

FetchContent_MakeAvailable(SFML)


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


# ============================================================
# Executable
# ============================================================

add_executable(main
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(main
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(main
    PRIVATE
        SFML::Graphics
        SFML::Audio
)


# ============================================================
# Development
# ============================================================

# Copy assets next to the executable after every build.
#
# This is intended for local development.
# Release packages use install() instead.

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:main>/assets"
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
            PROJECT1_GCC_SEARCH_DIRS
        ERROR_VARIABLE
            PROJECT1_GCC_SEARCH_ERROR
        RESULT_VARIABLE
            PROJECT1_GCC_SEARCH_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT PROJECT1_GCC_SEARCH_RESULT EQUAL 0)
        message(FATAL_ERROR
            "Could not determine MinGW runtime library paths.\n"
            "Compiler: ${CMAKE_CXX_COMPILER}\n\n"
            "${PROJECT1_GCC_SEARCH_ERROR}"
        )
    endif()

    string(
        REGEX MATCH
        "libraries:[^\n]*"
        PROJECT1_GCC_LIBRARIES_LINE
        "${PROJECT1_GCC_SEARCH_DIRS}"
    )

    if(NOT PROJECT1_GCC_LIBRARIES_LINE)
        message(FATAL_ERROR
            "Could not determine GCC library search paths."
        )
    endif()

    string(
        REGEX REPLACE
        "^libraries:[ \t]*=[ \t]*"
        ""
        PROJECT1_GCC_LIBRARY_PATHS
        "${PROJECT1_GCC_LIBRARIES_LINE}"
    )

    string(
        REPLACE
        ":"
        ";"
        PROJECT1_GCC_LIBRARY_PATHS
        "${PROJECT1_GCC_LIBRARY_PATHS}"
    )

    set(PROJECT1_RUNTIME_DIRECTORIES)

    foreach(PROJECT1_PATH IN LISTS PROJECT1_GCC_LIBRARY_PATHS)

        if(IS_DIRECTORY "${PROJECT1_PATH}")

            list(
                APPEND
                PROJECT1_RUNTIME_DIRECTORIES
                "${PROJECT1_PATH}"
            )

        endif()

    endforeach()

    list(
        REMOVE_DUPLICATES
        PROJECT1_RUNTIME_DIRECTORIES
    )

    message(STATUS "MinGW runtime search directories:")

    foreach(PROJECT1_PATH IN LISTS PROJECT1_RUNTIME_DIRECTORIES)

        message(STATUS "  ${PROJECT1_PATH}")

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
        TARGETS main
        RUNTIME_DEPENDENCY_SET Project1Runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

else()

    install(
        TARGETS main
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endif()


# ------------------------------------------------------------
# Install Windows runtime dependencies
# ------------------------------------------------------------

if(MINGW AND CMAKE_SYSTEM_NAME STREQUAL "Windows")

    install(
        RUNTIME_DEPENDENCY_SET Project1Runtime

        DESTINATION
            ${CMAKE_INSTALL_BINDIR}

        DIRECTORIES
            ${PROJECT1_RUNTIME_DIRECTORIES}

        PRE_EXCLUDE_REGEXES
            "^(advapi32|gdi32|kernel32|opengl32|user32|winmm|msvcrt)\\.dll$"
            "^api-ms-win-.*"
            "^ext-ms-win-.*"
    )

endif()


# ------------------------------------------------------------
# Install assets
# ------------------------------------------------------------

install(
    DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
    DESTINATION
        "${CMAKE_INSTALL_BINDIR}/assets"
)


# ============================================================
# Packaging
# ============================================================

include(CPack)

set(CPACK_PACKAGE_NAME
    "Project1"
)

set(CPACK_PACKAGE_VERSION
    "${PROJECT_VERSION}"
)

set(CPACK_PACKAGE_VENDOR
    "fsliwa"
)

set(CPACK_GENERATOR
    "ZIP"
)