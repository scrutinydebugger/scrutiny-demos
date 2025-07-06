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
#include <limits>

extern "C"
{
#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"
}

#define FORWARD_TO_ASCLIN_1 1 // For debugging purpose. asclin0 doe snot have physical headers on the board

// Match the fifo size, we should be free of overrun
uint8_t scrutiny_rx_buffer[BOARD_ASCLIN0_RX_BUFFER_SIZE];
uint8_t scrutiny_tx_buffer[BOARD_ASCLIN0_TX_BUFFER_SIZE];
uint8_t scrutiny_datalogging_buffer[32768]; // Allow as much as possible. Could use the linker script to allocate the "remaining memory"
scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler("Idle");
scrutiny::FixedFrequencyLoopHandler task_lowfreq_loop_handler(1e7 / 1000, "Low Freq");    // 1KHz
scrutiny::FixedFrequencyLoopHandler task_highfreq_loop_handler(1e7 / 10000, "High Freq"); // 10 KHz
scrutiny::LoopHandler *scrutiny_loops[] = {
    &task_idle_loop_handler,
    &task_lowfreq_loop_handler,
    &task_highfreq_loop_handler,
};

static scrutiny::Config config;
static scrutiny::MainHandler main_handler;

/// @brief Function that initializes the scrutiny library for this demo.
void configure_scrutiny()
{
    config.set_buffers(
        scrutiny_rx_buffer,
        sizeof(scrutiny_rx_buffer), // Receive
        scrutiny_tx_buffer,
        sizeof(scrutiny_tx_buffer) // Transmit
    );
    config.display_name = "aurix-tc344 demo";
    config.set_datalogging_buffers(scrutiny_datalogging_buffer, sizeof(scrutiny_datalogging_buffer));
    config.set_loops(scrutiny_loops, sizeof(scrutiny_loops) / sizeof(scrutiny_loops[0]));
    config.set_datalogging_trigger_callback(toggle_graph_trigger_pin);

    main_handler.init(&config);
}

/// @brief Function to be called periodically, as fast as possible
/// @param timestep_100ns The amount of time, in step of 100ns, since the last call to this function
void process_scrutiny_main(uint32_t const timestep_100ns)
{
    IfxAsclin_Asc *const scrutiny_asclin = &g_asclin0;
    uint8_t buffer[32]; // Temporary buffer
    Ifx_SizeT count;

    static_assert(std::numeric_limits<Ifx_SizeT>::max() > sizeof(buffer));
    count = IfxAsclin_Asc_getReadCount(scrutiny_asclin);
    count = Ifx__minu(count, sizeof(buffer));
    if (count > 0)
    {
        IfxAsclin_Asc_read(scrutiny_asclin, buffer, &count, 0);
#if FORWARD_TO_ASCLIN_1
        // For debug purpose with a logic analyzer. We can see what Scrutiny receives
        int16_t count2 = count;
        IfxAsclin_Asc_write(&g_asclin1, buffer, &count2, 0);
#endif
        main_handler.receive_data(buffer, static_cast<uint16_t>(count));
    }

    main_handler.process(timestep_100ns);

    uint16_t to_send{ static_cast<uint16_t>(main_handler.data_to_send()) };
    to_send = Ifx__minu(to_send, sizeof(buffer));
    if (to_send > 0)
    {
        count = to_send;
        main_handler.pop_data(buffer, to_send);
        IfxAsclin_Asc_write(scrutiny_asclin, buffer, &count, 0);
    }
}