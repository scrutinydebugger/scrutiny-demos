#!/bin/bash

#    build.sh
#        A script that invokes Aurix GCC compiler provided with AURIX Development Studio
#        Make sure to have the compiler, cmake and make in your paths
#
#   - License : MIT - See LICENSE file.
#   - Project :  Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
#
#   Copyright (c) 2025 Scrutiny Debugger

set -euo pipefail

SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
ROOT_DIR=$SCRIPT_DIR/..
cd "$ROOT_DIR" 

#rm -rf build
mkdir -p build
cd build


USE_CCACHE=0    # Use ccache if available
CCACHE=$(which "ccache" 2>/dev/null || true)
CCACHE_ARG=
if [[ ! -z $CCACHE && $USE_CCACHE -eq 1 ]]; then
    CCACHE_ARG=" -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"
    echo "Building with ccache at: $CCACHE";
fi

CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release}

cmake  \
    -G "Unix Makefiles" \
    -DCMAKE_VERBOSE_MAKEFILE=0 \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}  \
    $CCACHE_ARG \
    ..
cmake --build . -j 12