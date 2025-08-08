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

project(mx-snapshot
    VERSION 25.7
    DESCRIPTION "MX Snapshot - A tool for creating live ISO images from running MX Linux systems"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Find Qt6 components - include all components needed for either build
set(QT_COMPONENTS Core LinguistTools)
if(BUILD_GUI)
    list(APPEND QT_COMPONENTS Gui Widgets)
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
    src/settings.cpp
    src/batchprocessing.cpp
    src/work.cpp
    src/filesystemutils.cpp
    src/systeminfo.cpp
)

set(COMMON_HEADERS
    src/common.h
    src/log.h
    src/cmd.h
    src/settings.h
    src/batchprocessing.h
    src/work.h
    src/filesystemutils.h
    src/systeminfo.h
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

set(TRANSLATION_FILES
    translations/mx-snapshot_en.ts
)

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
    )

    # Release-specific optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(${target} PRIVATE NDEBUG)
        target_compile_options(${target} PRIVATE -O3)
        
        # Only add LTO for GCC, not Clang
        if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT USE_CLANG)
            target_compile_options(${target} PRIVATE -flto=auto)
            target_link_options(${target} PRIVATE -flto=auto)
        endif()
    endif()
endfunction()

# Apply common properties to targets
if(BUILD_GUI)
    set_common_properties(mx-snapshot)
endif()

if(BUILD_CLI)
    set_common_properties(iso-snapshot-cli)
endif()

# Handle translations
if(BUILD_GUI)
    qt6_add_translations(mx-snapshot
        TS_FILES ${TRANSLATION_FILES}
    )
endif()

if(BUILD_CLI)
    qt6_add_translations(iso-snapshot-cli
        TS_FILES ${TRANSLATION_FILES}
    )
endif()