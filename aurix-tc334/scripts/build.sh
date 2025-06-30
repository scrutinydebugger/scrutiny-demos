#!/bin/bash
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

cmake  \
    -G "Unix Makefiles" \
    -DCMAKE_VERBOSE_MAKEFILE=0 \
    $CCACHE_ARG \
    ..
cmake --build . -j 12