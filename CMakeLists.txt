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
# Windows cross-compilation runtime dependencies
#
# When building Windows binaries from another operating system
# using MinGW, ntldd is used to locate the required DLLs.
# ------------------------------------------------------------

set(PROJECT1_WINDOWS_CROSS_COMPILE
    FALSE
)

if(
    CMAKE_CROSSCOMPILING
    AND CMAKE_SYSTEM_NAME STREQUAL "Windows"
)

    set(PROJECT1_WINDOWS_CROSS_COMPILE
        TRUE
    )

    find_program(
        NTLDD_EXECUTABLE
        ntldd
        REQUIRED
    )

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallWindowsRuntime.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/InstallWindowsRuntime.cmake"
        @ONLY
    )

endif()


# ------------------------------------------------------------
# Install executable
# ------------------------------------------------------------

install(
    TARGETS main
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


# ------------------------------------------------------------
# Install assets
# ------------------------------------------------------------

install(
    DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
    DESTINATION
        "${CMAKE_INSTALL_BINDIR}/assets"
)


# ------------------------------------------------------------
# Install Windows runtime dependencies
# ------------------------------------------------------------

if(PROJECT1_WINDOWS_CROSS_COMPILE)

    install(
        SCRIPT
            "${CMAKE_CURRENT_BINARY_DIR}/InstallWindowsRuntime.cmake"
    )

endif()


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