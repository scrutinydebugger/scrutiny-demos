//    scrutiny_integration.hpp
//        Glue code between the Scrutiny library and the ESP32 application
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)

#ifndef SCRUTINY_INTEGRATION_HPP
#define SCRUTINY_INTEGRATION_HPP

#include <stdint.h>
#include "scrutiny.hpp"

void esp32_demo_config_scrutiny();
void esp32_demo_update_scrutiny(uint32_t const timestep_100ns);

extern scrutiny::MainHandler scrutiny_main;

#endif // SCRUTINY_INTEGRATION_HPP
