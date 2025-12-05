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
#include "usbd_cdc_if.h"

uint8_t scrutiny_rx_buffer[256];
uint8_t scrutiny_tx_buffer[512];
uint8_t scrutiny_datalogging_buffer[16384];

static scrutiny_c_config_t *scrutiny_config = NULL;
static scrutiny_c_main_handler_t *main_handler = NULL;
scrutiny_c_loop_handler_ff_t *task_1khz_loop_handler = NULL;
static scrutiny_c_loop_handler_vf_t *task_idle_loop_handler = NULL;
static scrutiny_c_loop_handler_t *scrutiny_loops[2] = { NULL, NULL };

static uint32_t last_timestamp_us;

#define RETURN_1_IF_NULL(x)                                                                                                                          \
    if ((x) == NULL)                                                                                                                                 \
    {                                                                                                                                                \
        return 1;                                                                                                                                    \
    }

int scrutiny_integration_init()
{
    // We use dynamic allocation because SCRUTINY_C_<TYPE>_SIZE are runtime values.
    // We could use static allocation with prior knowledge of these constant.
    // They can either be extracted with objdump or printed once, then replaced by a hardcoded value.
    // Note: Every "_construct" methods require a buffer of size equal or greater than their resepctive SIZE constant, so a bigger buffer
    // can be given, including some margin for future proofing the code.

    main_handler = scrutiny_c_main_handler_construct(malloc(SCRUTINY_C_MAIN_HANDLER_SIZE), SCRUTINY_C_MAIN_HANDLER_SIZE);
    RETURN_1_IF_NULL(main_handler);
    scrutiny_config = scrutiny_c_config_construct(malloc(SCRUTINY_C_CONFIG_SIZE), SCRUTINY_C_CONFIG_SIZE);
    RETURN_1_IF_NULL(scrutiny_config);
    task_idle_loop_handler =
        scrutiny_c_loop_handler_variable_freq_construct(malloc(SCRUTINY_C_LOOP_HANDLER_VF_SIZE), SCRUTINY_C_LOOP_HANDLER_VF_SIZE, "Idle loop");
    RETURN_1_IF_NULL(task_idle_loop_handler);
    task_1khz_loop_handler =
        scrutiny_c_loop_handler_fixed_freq_construct(malloc(SCRUTINY_C_LOOP_HANDLER_FF_SIZE), SCRUTINY_C_LOOP_HANDLER_FF_SIZE, 1e7 / 1000, "1KHz");
    RETURN_1_IF_NULL(task_1khz_loop_handler);

    scrutiny_loops[0] = task_idle_loop_handler;
    scrutiny_loops[1] = task_1khz_loop_handler;

    scrutiny_c_config_set_buffers(scrutiny_config, scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer), scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer));
    scrutiny_c_config_set_datalogging_buffers(scrutiny_config, scrutiny_datalogging_buffer, sizeof(scrutiny_datalogging_buffer));
    scrutiny_c_config_set_loops(scrutiny_config, scrutiny_loops, sizeof(scrutiny_loops) / sizeof(scrutiny_loops[0]));
    scrutiny_c_config_set_display_name(scrutiny_config, "STM32F4 Demo");

    if (scrutiny_c_main_handler_init(main_handler, scrutiny_config) != SCRUTINY_C_SUCCESS)
    {
        return 1;
    }
    return 0;
}

void scrutiny_integration_update(const uint32_t timestamp_us)
{
    uint32_t timestep_100ns = 10 * (timestamp_us - last_timestamp_us);
    last_timestamp_us = timestamp_us;

    scrutiny_c_loop_handler_variable_freq_process(task_idle_loop_handler, timestep_100ns);

    uint8_t rxTxBuf[CDC_QUEUE_MAX_PACKET_SIZE];
    uint16_t readSize = CDC_ReceiveQueue_ReadSize(&ReceiveQueue);
    if (readSize)
    {
        if (readSize > sizeof(rxTxBuf))
        {
            readSize = sizeof(rxTxBuf);
        }
        CDC_ReceiveQueue_Read(&ReceiveQueue, rxTxBuf, sizeof(rxTxBuf));
        CDC_resume_receive();

        scrutiny_c_main_handler_receive_data(main_handler, rxTxBuf, readSize);
    }

    scrutiny_c_main_handler_process(main_handler, timestep_100ns);

    unsigned int lenToSend = (unsigned int)scrutiny_c_main_handler_data_to_send(main_handler);

    if (lenToSend > 0)
    {
        if (lenToSend > sizeof(rxTxBuf))
        {
            lenToSend = sizeof(rxTxBuf);
        }

        scrutiny_c_main_handler_pop_data(main_handler, rxTxBuf, lenToSend);
        CDC_TransmitQueue_Enqueue(&TransmitQueue, rxTxBuf, lenToSend);
        CDC_continue_transmit();
    }
}