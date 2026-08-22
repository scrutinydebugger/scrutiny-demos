#/bin/bash

# This script executes the Scrutiny postbuild steps using bash
# Usage : ./scrutiny_postbuild.sh <path_to_elf>
# Can be invoked by an IDE as a post-build step.
# Don't use this script if you use cmake, use the scrutiny_postbuild() function instead

set -euo pipefail   # Stop on any error
# Helper for logging
fatal() { >&2 echo -e "[Error] $1"; exit ${2:-1}; }
info()  { >&2 echo -e "[Info] $1";}

# Work from where the script is.
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR"

info "Running Scrutiny post build script : $(basename "${BASH_SOURCE[0]}")"
which scrutiny 2>&1>/dev/null || fatal "scrutiny not in PATH"   # Check if we have scrutiny in the PATH

ELFFILE=${1-""}     # Read command line arg

[ -z ${ELFFILE} ] && fatal "Missing .elf firmware path"         # Check var empty
ELFFILE=$(realpath "$ELFFILE")
[ ! -f ${ELFFILE} ] && fatal "File does not exist"              # Check file exists


BUILD_DIR=$(dirname "$ELFFILE")                                 # Absolute path
ELFFILE_BASENAME_NO_EXT=$(basename $(echo "${ELFFILE%.*}"))     # No extension
ELFFILE_TAGGED_NAME="${ELFFILE_BASENAME_NO_EXT}_tagged.elf"
SFD_FILENAME="${ELFFILE_BASENAME_NO_EXT}.sfd"
cd $BUILD_DIR                                                   # Work next to the binary

WORKFOLDER="sfd_workfolder"
rm -rf "${WORKFOLDER}"
mkdir "${WORKFOLDER}"

scrutiny get-firmware-id "${ELFFILE}" --output "${WORKFOLDER}"  # Extract the firmware ID from the .elf and write to "<workfolder>/firmwareid"
scrutiny tag-firmware-id "${ELFFILE}" "${ELFFILE_TAGGED_NAME}"  # Inject the firmware ID inside the .elf
# Create the varmap file by reading the debug symbols
scrutiny elf2varmap "${ELFFILE}"  \
    --output "${WORKFOLDER}" \
    --dereference-pointers
scrutiny add-alias ${WORKFOLDER} --file ../aliases/app.json ../aliases/wavegen_alias.json
scrutiny make-metadata --project "STM32 CubeIDE RTT Demo" --version "1.0" --output "${WORKFOLDER}"   # Inject some metadata to identify the SFD more easily.
scrutiny make-sfd "${WORKFOLDER}" "${SFD_FILENAME}"  --install                              # Create the final .sfd file

info "Scrutiny post build script executed successfully!"
info "  - Tagged firmware: ${BUILD_DIR}/${ELFFILE_TAGGED_NAME}"
info "  - SFD file: ${BUILD_DIR}/${SFD_FILENAME}"
