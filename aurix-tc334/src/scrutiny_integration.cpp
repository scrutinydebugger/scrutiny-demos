//    scrutiny_integration.cpp
//        The app wrapper that integrates scrutiny in this demo by connecting the AscLin module
//        (UART) to the scrutiny streams.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

#include "board.hpp"
#include "scrutiny.hpp"
#include <cstdint>
// #include <limits>

extern "C"
{
#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"
}

uint8_t scrutiny_rx_buffer[64];
uint8_t scrutiny_tx_buffer[BOARD_ASCLIN0_TX_BUFFER_SIZE]; // Match the fifo size, we should be free of overrun
uint8_t scrutiny_datalogging_buffer[4096];                // Allow as much as possible. Could use the linker script to allocate the "remaining memory"
scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler("Idle");
scrutiny::FixedFrequencyLoopHandler task_lowfreq_loop_handler(1e7 / 1000);   // 1KHz
scrutiny::FixedFrequencyLoopHandler task_highfreq_loop_handler(1e7 / 10000); // 10 KHz
scrutiny::LoopHandler *scrutiny_loops[] = {
    &task_idle_loop_handler,
    &task_lowfreq_loop_handler,
    &task_highfreq_loop_handler,
};

static scrutiny::Config config;
static scrutiny::MainHandler main_handler;
static IfxAsclin_Asc *s_scrutiny_asclin;

/// @brief Function that initializes the scrutiny library for this demo.
void configure_scrutiny(IfxAsclin_Asc *const asclin)
{
    s_scrutiny_asclin = asclin;
    config.set_buffers(
        scrutiny_rx_buffer,
        sizeof(scrutiny_rx_buffer), // Receive
        scrutiny_tx_buffer,
        sizeof(scrutiny_tx_buffer) // Transmit
    );
    config.display_name = "aurix-tc344 demo";
    config.set_datalogging_buffers(scrutiny_datalogging_buffer, sizeof(scrutiny_datalogging_buffer));
    config.set_loops(scrutiny_loops, sizeof(scrutiny_loops) / sizeof(scrutiny_loops[0]));

    main_handler.init(&config);
}

/// @brief Function to be called periodically, as fast as possible
/// @param timestep_100ns The amount of time, in step of 100ns, since the last call to this function
void process_scrutiny_main(uint32_t const timestep_100ns)
{
    uint8_t buffer[32];
    Ifx_SizeT count;

    // static_assert(std::numeric_limits<Ifx_SizeT>::max() > sizeof(buffer));
    count = sizeof(buffer);
    IfxAsclin_Asc_read(s_scrutiny_asclin, buffer, &count, 0);
    main_handler.receive_data(buffer, static_cast<uint16_t>(count));

    main_handler.process(timestep_100ns);

    uint16_t toSend{ static_cast<uint16_t>(main_handler.data_to_send()) };
    if (toSend > sizeof(buffer))
    {
        toSend = sizeof(buffer);
    }
    count = toSend;
    main_handler.pop_data(buffer, toSend);
    IfxAsclin_Asc_write(s_scrutiny_asclin, buffer, &count, 0);
}