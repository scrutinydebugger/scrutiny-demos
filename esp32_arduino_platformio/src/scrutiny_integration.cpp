//    scrutiny_integration.cpp
//        Glue code between the Scrutiny library and the ESP32 application
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)

#include "scrutiny_integration.hpp"
#include <Arduino.h>

static uint8_t scrutiny_rx_buffer[128];
static uint8_t scrutiny_tx_buffer[128];
static uint8_t scrutiny_datalogging_buffer[4096];

scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler("MainLoop");
scrutiny::LoopHandler *scrutiny_loops[] = {
    &task_idle_loop_handler
};

static scrutiny::Config config;
scrutiny::MainHandler scrutiny_main;

void esp32_demo_config_scrutiny()
{
    config.set_buffers(
        scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer),
        scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer)
    );
    config.display_name = "ESP32 PlatformIO Demo";
    config.set_datalogging_buffers(scrutiny_datalogging_buffer, sizeof(scrutiny_datalogging_buffer));
    config.set_loops(scrutiny_loops, sizeof(scrutiny_loops) / sizeof(scrutiny_loops[0]));

    scrutiny_main.init(&config);
}

void esp32_demo_update_scrutiny(uint32_t const timestep_100ns)
{
    // 1. Ingest incoming UART bytes
    uint8_t buffer[64];
    int bytes_avail = Serial.available();
    if (bytes_avail > 0) {
        int to_read = min((int)sizeof(buffer), bytes_avail);
        int nrx = Serial.readBytes((char*)buffer, to_read);
        if (nrx > 0) {
            scrutiny_main.receive_data(buffer, (uint16_t)nrx);
        }
    }

    // 2. Advance Scrutiny timebase and process loop
    task_idle_loop_handler.process(timestep_100ns);
    scrutiny_main.process(timestep_100ns);

    // 3. Drain outgoing UART bytes
    uint16_t available = scrutiny_main.data_to_send();
    if (available > 0) {
        uint16_t to_send = min((uint16_t)sizeof(buffer), available);
        uint16_t ntx = scrutiny_main.pop_data(buffer, to_send);
        if (ntx > 0) {
            Serial.write(buffer, ntx);
        }
    }
}
