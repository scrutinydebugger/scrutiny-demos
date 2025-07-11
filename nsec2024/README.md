# NorthSec 2024 Demo (Arduino Mega 2560)

This project is a Arduino project used to demonstrate the Scrutiny framework during the 2024 edition of the NorthSec conference (Montreal)

https://nsec.io/session/2024-scrutiny-debugger-debug-test-and-configure-embedded-softwares-through-instrumentation.html

Talk: https://www.youtube.com/live/r30Ci3DNy5A?feature=shared&t=9214

**IMPORTANT**: The demo has been converted to a more generic Arduino demo. See [atmega_2560_cmake](../atmega_2560_cmake)

Since that demo many thing changed, namely:

 - The GUI is QT based, not Electron based
 - The communication between the server and the client is not websockets anymore
 - The GUI uses the SDK to communicate with the server