#!/bin/bash

set -xeuo pipefail
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR"
git submodule update --init scrutiny-embedded # Fetch scrutiny-embedded

# Copy the default config if we don't already have one.
[ ! -f "./scrutiny_build_config.hpp" ] && cp scrutiny-embedded/lib/inc/scrutiny_build_config.hpp.default ./scrutiny_build_config.hpp