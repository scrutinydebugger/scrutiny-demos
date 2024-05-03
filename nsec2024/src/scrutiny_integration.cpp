//    scrutiny_integration.cpp
//        Glue code between the scrutiny lib and the application
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2024 Scrutiny Debugger

#include "scrutiny.hpp"
#include <stdint.h>
#include <Arduino.h>

uint8_t scrutiny_rx_buffer[64];
uint8_t scrutiny_tx_buffer[128];
uint8_t scrutiny_datalogging_buffer[4096];  // Allow as much as possible. Could use the linker script to allocate the "remaining memory"
scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler("Idle");
scrutiny::FixedFrequencyLoopHandler task_100hz_loop_handler(1e7/100); 
scrutiny::FixedFrequencyLoopHandler task_20hz_loop_handler(1e7/20); 
scrutiny::LoopHandler *scrutiny_loops[] = {
    &task_idle_loop_handler,
    &task_100hz_loop_handler,
    &task_20hz_loop_handler,
};

static scrutiny::Config config;
static scrutiny::MainHandler main_handler;

/// @brief Function that initializes the scrutiny library for this demo.
void nsec2024_demo_config_scrutiny()
{
    config.set_buffers(
        scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer),     // Receive
        scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer)      // Transmit 
    );
    config.display_name = "nsec2024_demo";
    config.set_datalogging_buffers(scrutiny_datalogging_buffer, sizeof(scrutiny_datalogging_buffer));
    config.set_loops(scrutiny_loops, sizeof(scrutiny_loops) / sizeof(scrutiny_loops[0]));

    main_handler.init(&config);
}

/// @brief Function to be called periodically, as fast as possible
/// @param timestep_100ns The amount of time, in step of 100ns, since the last call to this function
void nsec2024_demo_update_scrutiny_main(uint32_t const timestep_100ns)
{
    uint8_t buffer[16];
    unsigned int toRead{ static_cast<unsigned int>(Serial.available())};
    if (toRead > sizeof(buffer)){
        toRead = sizeof(buffer);
    }

    uint16_t const nrx{ static_cast<uint16_t>(Serial.readBytes(buffer, toRead)) };
    main_handler.comm()->receive_data(buffer, nrx);  
    
    main_handler.process(timestep_100ns);

    unsigned int toSend{ static_cast<unsigned int>( main_handler.comm()->data_to_send())};
    if (toSend > sizeof(buffer)){
        toSend = sizeof(buffer);
    }
    main_handler.comm()->pop_data(buffer, toSend);
    Serial.write(buffer, toSend);
}