cmake_minimum_required(VERSION 3.28)

project(Project1
    VERSION 1.0
    LANGUAGES CXX
)

# ============================================================
# C++ configuration
# ============================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Executables created by the build go to build/bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# SFML
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
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(main
    PRIVATE
        SFML::Graphics
)


# ============================================================
# Installation
# ============================================================

include(GNUInstallDirs)

# Copy executable to install/bin/
install(
    TARGETS main
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Copy assets to install/bin/assets/
install(
    DIRECTORY assets/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/assets
)


# ============================================================
# Development helper
# ============================================================

# Build + install the project with one command:
#
# cmake --build build --target install_project
#
add_custom_target(install_project
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --prefix ${CMAKE_SOURCE_DIR}/install
    DEPENDS main
)


# ============================================================
# Visual Studio
# ============================================================

set_property(
    DIRECTORY
    PROPERTY VS_STARTUP_PROJECT main
)

# ============================================================
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:main>/assets
)
