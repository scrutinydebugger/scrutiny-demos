#/bin/bash 
set -euo pipefail
RED='\033[0;31m';  NC='\033[0m';
fatal() { >&2 echo -e "$RED[Error]$NC $1"; exit ${2:-1}; }

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR"

FIRMWARE=${1-""}

[ -z ${FIRMWARE} ] && fatal "Missing firmware path"
FIRMWARE=$(realpath $FIRMWARE)
[ ! -f ${FIRMWARE} ] && fatal "File does not exist"

FIRMWARE_BASENAME=$(echo "${FIRMWARE%.*}")

FIRMWARE_DIR=$(dirname "$FIRMWARE")
cd $FIRMWARE_DIR

which scrutiny 2>&1 > /dev/null || fatal "scrutiny not in PATH"

WORKFOLDER="sfd_workfolder"
rm -rf "${WORKFOLDER}"
mkdir "${WORKFOLDER}"

scrutiny get-firmware-id "${FIRMWARE}" --output "${WORKFOLDER}"
scrutiny tag-firmware-id "${FIRMWARE}" "${FIRMWARE_BASENAME}_tagged"
scrutiny elf2varmap "${FIRMWARE}" --output "${WORKFOLDER}" \
    --path_ignore_patterns  \
        "/global/typeinfo*" \
        "/global/vtable*"   \
        "*__vptr"           \
        "/global/__TI*"     \
        "/global/std/*"     \
        "/global/_sys_memory/*"
scrutiny make-metadata --project "LaunchXL-F280049C Demo" --version "1.0" --output "${WORKFOLDER}"
scrutiny make-sfd "${WORKFOLDER}" "${FIRMWARE_BASENAME}.sfd"  --install