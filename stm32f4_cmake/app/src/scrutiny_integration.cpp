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

#include "scrutiny_integration.h"

#include "scrutiny.hpp"
#include "usbd_cdc_if.h"

uint8_t scrutiny_rx_buffer[256];
uint8_t scrutiny_tx_buffer[512];
uint8_t scrutiny_datalogging_buffer[16384];
scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler("Idle");
scrutiny::FixedFrequencyLoopHandler task_100hz_loop_handler(1e7 / 100, "100Hz");
scrutiny::LoopHandler *scrutiny_loops[] = {
    &task_idle_loop_handler,
    &task_100hz_loop_handler,
};

constexpr uint32_t TASK_100HZ_TIME_US{ 1000000 / 100 };

static scrutiny::Config config;
static scrutiny::MainHandler main_handler;

static uint32_t last_timestamp_us;
static uint32_t last_timestamp_task_100hz;

void scrutiny_integration_init()
{
    config.set_buffers(scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer), scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer));
    config.set_datalogging_buffers(scrutiny_datalogging_buffer, sizeof(scrutiny_datalogging_buffer));
    config.set_loops(scrutiny_loops, sizeof(scrutiny_loops) / sizeof(scrutiny_loops[0]));
    config.display_name = "STM32 Demo";

    main_handler.init(&config);
}

void scrutiny_integration_update(const uint32_t timestamp_us)
{
    uint32_t timestep_100ns = 10 * (timestamp_us - last_timestamp_us);
    last_timestamp_us = timestamp_us;

    task_idle_loop_handler.process(timestep_100ns);

    if (timestamp_us - last_timestamp_task_100hz >= TASK_100HZ_TIME_US)
    {
        task_100hz_loop_handler.process();
        last_timestamp_task_100hz = timestamp_us;
    }

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