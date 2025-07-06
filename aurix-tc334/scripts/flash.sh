#!/bin/bash

#    flash.sh
#        A script to flash to produced binary in the kit_a2g_tc334_lite dev board
#
#   - License : MIT - See LICENSE file.
#   - Project :  Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
#
#   Copyright (c) 2024 Scrutiny Debugger

set -euo pipefail

SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
ROOT_DIR=$SCRIPT_DIR/..
cd "$ROOT_DIR" 

AURIXFlasher -elf build/bin/scrutiny-aurix-tc334-tagged.elf