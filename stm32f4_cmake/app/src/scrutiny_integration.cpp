//    scrutiny_integration.cpp
//        The app wrapper that integrates scrutiny in this demo by connecting the USB virtual
//        serial port (CDC-ACM) to the scrutiny streams.
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//   - Author : MrMati (Mateusz Niedbała) <mati.niedbala5@gmail.com>
//   - Contributors :
//       - Pier-Yves Lessard (pylessard)
//
//    Copyright (c) 2025 Scrutiny Debugger

#include "scrutiny_integration.hpp"

#include "scrutiny.hpp"
#include "usbd_cdc_if.h"

uint8_t scrutiny_rx_buffer[256];
uint8_t scrutiny_tx_buffer[512];
uint8_t scrutiny_datalogging_buffer[16384];
scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler("Idle");
scrutiny::FixedFrequencyLoopHandler task_1khz_loop_handler(1e7 / 1000, "1KHz");
scrutiny::LoopHandler *scrutiny_loops[] = {
    &task_idle_loop_handler,
    &task_1khz_loop_handler,
};

static scrutiny::Config config;
static scrutiny::MainHandler main_handler;

static uint32_t last_timestamp_us;
static uint32_t last_timestamp_task_100hz;

void scrutiny_integration_init()
{
    config.set_buffers(scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer), scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer));
    config.set_datalogging_buffers(scrutiny_datalogging_buffer, sizeof(scrutiny_datalogging_buffer));
    config.set_loops(scrutiny_loops, sizeof(scrutiny_loops) / sizeof(scrutiny_loops[0]));
    config.display_name = "STM32F4 Demo";

    main_handler.init(&config);
}

void scrutiny_integration_update(const uint32_t timestamp_us)
{
    uint32_t timestep_100ns = 10 * (timestamp_us - last_timestamp_us);
    last_timestamp_us = timestamp_us;

    task_idle_loop_handler.process(timestep_100ns);

    uint8_t rxTxBuf[CDC_QUEUE_MAX_PACKET_SIZE];
    uint16_t readSize = CDC_ReceiveQueue_ReadSize(&ReceiveQueue);
    if (readSize)
    {
        if (readSize > sizeof(rxTxBuf))
            readSize = sizeof(rxTxBuf);
        CDC_ReceiveQueue_Read(&ReceiveQueue, rxTxBuf, sizeof(rxTxBuf));
        CDC_resume_receive();

        main_handler.receive_data(rxTxBuf, readSize);
    }

    main_handler.process(timestep_100ns);

    unsigned int lenToSend{ static_cast<unsigned int>(main_handler.data_to_send()) };

    if (lenToSend > 0)
    {
        if (lenToSend > sizeof(rxTxBuf))
            lenToSend = sizeof(rxTxBuf);

        main_handler.pop_data(rxTxBuf, lenToSend);
        CDC_TransmitQueue_Enqueue(&TransmitQueue, rxTxBuf, lenToSend);
        CDC_continue_transmit();
    }
}