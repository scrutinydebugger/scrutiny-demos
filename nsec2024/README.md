# NorthSec 2024 Demo

This project is a Arduino project used to demonstrate the Scrutiny framework during the 2024 edition of NorthSec


https://nsec.io/session/2024-scrutiny-debugger-debug-test-and-configure-embedded-softwares-through-instrumentation.html


## Building the Arduino app

Calling the following script should be enough, assuming you have docker configured

```bash
export ARDUINO_PORT=/dev/ttyACM0    # Change serial port
./scripts/build_in_docker
./scripts/flash
```

## Running the server

```bash
# pip install scrutinydebugger # For latest release
# scrutiny-python repo has a tag "nsec2024_demo" that points on the version used.
git clone --depth 1 -b nsec2024_demo https://github.com/scrutinydebugger/scrutiny-python /tmp/scrutiny-python && pip3 install -e /tmp/scrutiny-python
scrutiny launch-server --config scrutiny_server_config.json # Make sure to adjust the serial port name in that configuration
```

