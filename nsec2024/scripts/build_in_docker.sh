#!/bin/bash
set -euo pipefail 

SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
ROOT_DIR=$SCRIPT_DIR/..
cd $ROOT_DIR
WORKDIR=$ROOT_DIR     # Having Workdir = script dir makes the compile_commands.json paths match the host system. Easier for vscode integration
IMAGE_NAME=scrutiny_nsec2024_demo
CACHE_OPT=
while getopts "f" o; do
    case "${o}" in
        f) CACHE_OPT="--no-cache";; # Force option rebuilds the image
        *);;
    esac
done

docker build . -t $IMAGE_NAME $CACHE_OPT
docker run \
    --volume $ROOT_DIR:$WORKDIR \
    -e ARDUINO_PATH=/usr/share/arduino \
    -u $(id -u):$(id -g) \
    $IMAGE_NAME \
    bash -c $WORKDIR/scripts/build.sh