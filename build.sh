#!/bin/bash

# **********************************************************************
# * Copyright (C) 2015-2025 MX Authors
# *
# * Authors: Adrian
# *          MX Linux <http://mxlinux.org>
# *
# * This file is part of mx-snapshot.
# *
# * mx-snapshot is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * mx-snapshot is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with mx-snapshot.  If not, see <http://www.gnu.org/licenses/>.
# **********************************************************************/

set -e

# Default values
BUILD_DIR="build"
BUILD_TYPE="Release"
USE_CLANG=false
CLEAN=false
DEBIAN_BUILD=false
BUILD_GUI=true
BUILD_CLI=true

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clang)
            USE_CLANG=true
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --debian)
            DEBIAN_BUILD=true
            shift
            ;;
        --gui-only)
            BUILD_GUI=true
            BUILD_CLI=false
            shift
            ;;
        --cli-only)
            BUILD_GUI=false
            BUILD_CLI=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  -d, --debug     Build in Debug mode (default: Release)"
            echo "  -c, --clang     Use clang compiler"
            echo "  --clean         Clean build directory before building"
            echo "  --debian        Build Debian package"
            echo "  --gui-only      Build only GUI version (mx-snapshot)"
            echo "  --cli-only      Build only CLI version (iso-snapshot-cli)"
            echo "  -h, --help      Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Build Debian package
if [ "$DEBIAN_BUILD" = true ]; then
    echo "Building Debian package..."
    debuild -us -uc

    echo "Creating debs directory and moving debian artifacts..."
    mkdir -p debs
    mv ../*.deb debs/ 2>/dev/null || true
    mv ../*.changes debs/ 2>/dev/null || true  
    mv ../*.dsc debs/ 2>/dev/null || true
    mv ../*.tar.* debs/ 2>/dev/null || true
    mv ../*.buildinfo debs/ 2>/dev/null || true
    mv ../*build* debs/ 2>/dev/null || true

    echo "Cleaning build directory and debian artifacts..."
    rm -rf "$BUILD_DIR"
    rm -f debian/*.debhelper.log debian/*.substvars debian/files
    rm -rf debian/.debhelper/ debian/mx-snapshot/ debian/iso-snapshot-cli/ obj-*/
    rm -f translations/*.qm src/version.h
    rm -f ../*build* ../*.buildinfo 2>/dev/null || true

    echo "Debian package build completed!"
    echo "Debian artifacts moved to debs/ directory"
    exit 0
fi

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    echo "Cleaning build directory and debian artifacts..."
    rm -rf "$BUILD_DIR"
    rm -f debian/*.debhelper.log debian/*.substvars debian/files
    rm -rf debian/.debhelper/ debian/mx-snapshot/ debian/iso-snapshot-cli/ obj-*/
    rm -f translations/*.qm src/version.h
    rm -f ../*build* ../*.buildinfo 2>/dev/null || true
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Configure CMake with Ninja
echo "Configuring CMake with Ninja generator..."
CMAKE_ARGS=(
    -G Ninja
    -B "$BUILD_DIR"
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    -DBUILD_GUI="$([ "$BUILD_GUI" = true ] && echo ON || echo OFF)"
    -DBUILD_CLI="$([ "$BUILD_CLI" = true ] && echo ON || echo OFF)"
)

if [ "$USE_CLANG" = true ]; then
    CMAKE_ARGS+=(-DUSE_CLANG=ON)
    echo "Using clang compiler"
fi

echo "Build configuration:"
echo "  GUI: $([ "$BUILD_GUI" = true ] && echo "enabled" || echo "disabled")"
echo "  CLI: $([ "$BUILD_CLI" = true ] && echo "enabled" || echo "disabled")"

cmake "${CMAKE_ARGS[@]}"

# Build the project
echo "Building project with Ninja..."
cmake --build "$BUILD_DIR" --parallel

echo "Build completed successfully!"
if [ "$BUILD_GUI" = true ]; then
    echo "GUI executable: $BUILD_DIR/mx-snapshot"
fi
if [ "$BUILD_CLI" = true ]; then
    echo "CLI executable: $BUILD_DIR/iso-snapshot-cli"
fi
