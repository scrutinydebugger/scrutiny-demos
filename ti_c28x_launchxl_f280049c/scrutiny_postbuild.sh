#/bin/bash 
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
ELFFILE_TAGGED_NAME="${ELFFILE_BASENAME_NO_EXT}_tagged.out" 
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
    --dereference-pointers  \
    --path_ignore_patterns  \
        "/global/typeinfo*" \
        "/global/vtable*"   \
        "*__vptr"           \
        "/global/__TI*"     \
        "/global/std/*"     \
        "/global/_sys_memory/*"
scrutiny add-alias ${WORKFOLDER} --file ../aliases/alias_hardware.json alias_plant.json
scrutiny make-metadata --project "C2000 Testapp" --version "1.0" --output "${WORKFOLDER}"   # Inject some metadata to identify the SFD more easily.
scrutiny make-sfd "${WORKFOLDER}" "${SFD_FILENAME}"  --install                              # Create the final .sfd file

info "Scrutiny post build script executed successfully!"
info "  - Tagged firmware: ${BUILD_DIR}/${ELFFILE_TAGGED_NAME}"
info "  - SFD file: ${BUILD_DIR}/${SFD_FILENAME}"