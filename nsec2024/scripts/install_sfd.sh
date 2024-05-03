#/bin/bash
SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
ROOT_DIR=$SCRIPT_DIR/..
cd "$ROOT_DIR" 

SFD_NAME=scrutiny-nsec2024.sfd
scrutiny install-sfd build/$SFD_NAME