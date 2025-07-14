#/bin/bash
SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
ROOT_DIR=$SCRIPT_DIR/..
cd "$ROOT_DIR" 
DEFAULT_PORT=/dev/ttyACM0

source $SCRIPT_DIR/common.sh

if [ -z ${ARDUINO_PORT:+x} ]; then
    warn "ARDUINO_PORT not set. Using $DEFAULT_PORT"
    ARDUINO_PORT=$DEFAULT_PORT
fi

# -hup prevents resetting the Arduino board on port open
interceptty $ARDUINO_PORT $ROOT_DIR/tty_debug  -v -T -o tty_debug.log -s "speed 115200 -hup"