# **********************************************************************
# * Copyright (C) 2015-2025 MX Authors
# *
# * Authors: Adrian
# *          MX Linux <http://mxlinux.org>
# *
# * This file is part of MX Snapshot.
# *
# * MX Snapshot is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * MX Snapshot is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with MX Tools.  If not, see <http://www.gnu.org/licenses/>.
# **********************************************************************

cmake_minimum_required(VERSION 3.16)

# Resolve project version with a fallback chain so the build works in:
#   - git checkouts (uses the latest tag)
#   - Debian source builds (uses dpkg-parsechangelog)
#   - Arch / source tarballs without git or dpkg (regex on debian/changelog)
# Override with -DGIT_TAG_VERSION=x.y.z to skip detection.
if(NOT GIT_TAG_VERSION)
    find_program(GIT_EXECUTABLE git)
    if(GIT_EXECUTABLE)
        # Match version-shaped tags only (e.g. 26.04.2), so non-version tags
        # like "pre-unify-main" don't pollute the project version.
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0 --match "[0-9]*"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TAG_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE GIT_RESULT
            ERROR_QUIET
        )
    endif()
endif()

if(NOT GIT_TAG_VERSION)
    find_program(DPKG_PARSECHANGELOG dpkg-parsechangelog)
    if(DPKG_PARSECHANGELOG)
        execute_process(
            COMMAND ${DPKG_PARSECHANGELOG} -SVersion
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TAG_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()
endif()

if(NOT GIT_TAG_VERSION AND EXISTS "${CMAKE_SOURCE_DIR}/debian/changelog")
    file(READ "${CMAKE_SOURCE_DIR}/debian/changelog" CHANGELOG_CONTENTS)
    string(REGEX MATCH "^[^ ]+ \\(([^)]+)\\)" _match "${CHANGELOG_CONTENTS}")
    set(GIT_TAG_VERSION "${CMAKE_MATCH_1}")
endif()

if(NOT GIT_TAG_VERSION)
    message(FATAL_ERROR "Failed to determine version from git tag, dpkg-parsechangelog, or debian/changelog")
endif()

project(mx-snapshot
    DESCRIPTION "MX Snapshot - A tool for creating live ISO images from running MX Linux systems"
    LANGUAGES CXX
)
set(PROJECT_VERSION "${GIT_TAG_VERSION}")

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile commands export for IDEs and tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimize for Ninja builds
if(CMAKE_GENERATOR STREQUAL "Ninja")
    # Enable colored output for Ninja
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

# Option to use clang for testing builds
option(USE_CLANG "Use clang compiler" OFF)
if(USE_CLANG)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_CXX_COMPILER_ID "Clang")
    message(STATUS "Using clang compiler")
endif()

# Options to control what to build
option(BUILD_GUI "Build GUI version" ON)
option(BUILD_CLI "Build CLI version" ON)
option(ARCH_BUILD "Build for Arch Linux" OFF)
option(BUILD_TESTS "Build test suite" OFF)

if(ARCH_BUILD)
    add_compile_definitions(ARCH_BUILD)
endif()

# Find Qt6 components - include all components needed for either build
set(QT_COMPONENTS Core LinguistTools)
if(BUILD_GUI)
    list(APPEND QT_COMPONENTS Gui Widgets)
endif()
if(BUILD_TESTS)
    list(APPEND QT_COMPONENTS Test Widgets)
endif()

find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})

# Enable automatic MOC, UIC, and RCC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Common source files
set(COMMON_SOURCES
    src/log.cpp
    src/main.cpp
    src/cmd.cpp
    src/elevationbroker.cpp
    src/settings.cpp
    src/batchprocessing.cpp
    src/work.cpp
    src/filesystemutils.cpp
    src/systeminfo.cpp
    src/messagehandler.cpp
    src/excludesutils.cpp
    src/squashfsutils.cpp
)

set(COMMON_HEADERS
    src/common.h
    src/log.h
    src/cmd.h
    src/elevationbroker.h
    src/settings.h
    src/batchprocessing.h
    src/work.h
    src/filesystemutils.h
    src/systeminfo.h
    src/messagehandler.h
    src/excludesutils.h
    src/squashfsutils.h
)

set(HELPER_SOURCES
    src/helper.cpp
)

# GUI-specific source files
set(GUI_SOURCES
    src/mainwindow.cpp
    src/about.cpp
)

set(GUI_HEADERS
    src/mainwindow.h
    src/about.h
)

set(UI_FILES
    src/mainwindow.ui
)

set(RESOURCE_FILES
    images.qrc
)

# Get all translation files
file(GLOB TRANSLATION_FILES "translations/*.ts")

# Create GUI executable
if(BUILD_GUI)
    add_executable(mx-snapshot
        ${COMMON_SOURCES}
        ${GUI_SOURCES}
        ${COMMON_HEADERS}
        ${GUI_HEADERS}
        ${UI_FILES}
        ${RESOURCE_FILES}
    )

    # Link Qt6 libraries for GUI
    target_link_libraries(mx-snapshot
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
    )
    set_target_properties(mx-snapshot PROPERTIES
        OUTPUT_NAME "mx-snapshot"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

add_executable(helper
    ${HELPER_SOURCES}
)

target_link_libraries(helper
    Qt6::Core
)

set_target_properties(helper PROPERTIES
    OUTPUT_NAME "helper"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Create CLI executable
if(BUILD_CLI)
    add_executable(iso-snapshot-cli
        ${COMMON_SOURCES}
        ${COMMON_HEADERS}
        ${RESOURCE_FILES}
    )

    # Set CLI build definition
    target_compile_definitions(iso-snapshot-cli PRIVATE CLI_BUILD=1)

    # Link Qt6 libraries for CLI
    target_link_libraries(iso-snapshot-cli
        Qt6::Core
    )
    set_target_properties(iso-snapshot-cli PROPERTIES
        OUTPUT_NAME "iso-snapshot-cli"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Function to set common compile options and definitions
function(set_common_properties target)
    # Set compiler flags
    target_compile_options(${target} PRIVATE
        -Wpedantic
        -pedantic
        -Werror=return-type
        -Werror=switch
        -Werror=uninitialized
        -Werror
    )

    # Add compiler-specific flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR USE_CLANG)
        target_compile_options(${target} PRIVATE -Werror=return-stack-address)
    else()
        target_compile_options(${target} PRIVATE -Werror=return-local-addr)
    endif()

    # Set compile definitions
    target_compile_definitions(${target} PRIVATE
        QT_DEPRECATED_WARNINGS
        QT_DISABLE_DEPRECATED_BEFORE=0x060000
        VERSION="${PROJECT_VERSION}"
    )

    # Build type specific optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(${target} PRIVATE NDEBUG)
        target_compile_options(${target} PRIVATE -O3)

        # Add LTO - different flags for different compilers
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR USE_CLANG)
            target_compile_options(${target} PRIVATE -flto=thin)
            target_link_options(${target} PRIVATE -flto=thin)
        else()
            target_compile_options(${target} PRIVATE -flto=auto)
            target_link_options(${target} PRIVATE -flto=auto)
        endif()
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE -g -O0)
        target_compile_definitions(${target} PRIVATE DEBUG)
    endif()
endfunction()

# Apply common properties to targets
if(BUILD_GUI)
    set_common_properties(mx-snapshot)
endif()

if(BUILD_CLI)
    set_common_properties(iso-snapshot-cli)
endif()

set_common_properties(helper)

if(BUILD_TESTS)
    enable_testing()

    add_executable(test-squashfsutils
        tests/test_squashfsutils.cpp
        src/squashfsutils.cpp
        src/squashfsutils.h
    )
    target_link_libraries(test-squashfsutils Qt6::Core Qt6::Test)
    target_include_directories(test-squashfsutils PRIVATE src)
    set_common_properties(test-squashfsutils)
    add_test(NAME squashfsutils COMMAND test-squashfsutils)

    add_executable(test-cmd
        tests/test_cmd.cpp
        src/cmd.cpp
        src/elevationbroker.cpp
        src/messagehandler.cpp
        src/cmd.h
        src/elevationbroker.h
        src/messagehandler.h
    )
    target_link_libraries(test-cmd Qt6::Core Qt6::Test Qt6::Widgets)
    target_include_directories(test-cmd PRIVATE src)
    set_common_properties(test-cmd)
    add_test(NAME cmd COMMAND test-cmd)

    add_executable(test-elevationbroker
        tests/test_elevationbroker.cpp
        src/elevationbroker.cpp
        src/elevationbroker.h
    )
    target_link_libraries(test-elevationbroker Qt6::Core Qt6::Test)
    target_include_directories(test-elevationbroker PRIVATE src)
    set_common_properties(test-elevationbroker)
    add_dependencies(test-elevationbroker helper)
    add_test(NAME elevationbroker COMMAND test-elevationbroker)
endif()

# Handle translations
if(BUILD_GUI)
    qt6_add_translations(mx-snapshot
        TS_FILES ${TRANSLATION_FILES}
	LRELEASE_OPTIONS -compress -nounfinished -removeidentical -silent
	QM_FILES_OUTPUT_VARIABLE qm_files
    )
endif()

if(BUILD_CLI)
    qt6_add_translations(iso-snapshot-cli
        TS_FILES ${TRANSLATION_FILES}
	LRELEASE_OPTIONS -compress -nounfinished -removeidentical -silent
	QM_FILES_OUTPUT_VARIABLE qm_files
    )
endif()



# Installation targets (required by Debian build system)
# Other files are handled by debian/install
if(BUILD_GUI)
    install(TARGETS mx-snapshot
        RUNTIME DESTINATION bin
    )
endif()

if(BUILD_CLI)
    install(TARGETS iso-snapshot-cli
        RUNTIME DESTINATION bin
    )
endif()
