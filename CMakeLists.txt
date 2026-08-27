cmake_minimum_required(VERSION 3.28)

project(Project1
    VERSION 1.0.0
    LANGUAGES CXX
)

# ============================================================
# Project configuration
# ============================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# All executables go to:
# build/<preset>/bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin
)


# ============================================================
# Dependencies
# ============================================================

include(FetchContent)

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

# Make assets available next to the executable after every build.
#
# This is only for local development.
# Release packaging uses install() below.

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:main>/assets"
)


# ============================================================
# Installation
# ============================================================

include(GNUInstallDirs)

# Install executable and its runtime dependencies.
#
# On Windows this is important because the executable may depend
# on SFML DLLs and other runtime libraries.

install(
    TARGETS main
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install game assets.
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
    DESTINATION "${CMAKE_INSTALL_BINDIR}/assets"
)


# ============================================================
# Packaging
# ============================================================

set(CPACK_PACKAGE_NAME "Project1")
set(CPACK_PACKAGE_VENDOR "fsliwa")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

# ZIP is portable and does not require an installer framework.
set(CPACK_GENERATOR "ZIP")

include(CPack)