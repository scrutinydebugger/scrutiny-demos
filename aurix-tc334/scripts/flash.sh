#!/bin/bash
set -euo pipefail

SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
ROOT_DIR=$SCRIPT_DIR/..
cd "$ROOT_DIR" 

AURIXFlasher -elf build/bin/scrutiny-aurix-tc334.elf