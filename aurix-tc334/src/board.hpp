//    board.hpp
//        The abstraction layer to configure the dev board (kit_a2g_tc334_lite)
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

#ifndef DEMO_TC334_BOARD_HPP
#define DEMO_TC334_BOARD_HPP

extern "C"
{
#include "IfxAsclin_Asc.h"
}

#include <cstdint>

#define BOARD_ASCLIN0_SERIAL_PIN_RX IfxAsclin0_RXA_P14_1_IN
#define BOARD_ASCLIN0_SERIAL_PIN_TX IfxAsclin0_TX_P14_0_OUT
#define BOARD_ASCLIN0_TX_BUFFER_SIZE 256
#define BOARD_ASCLIN0_RX_BUFFER_SIZE 128

#define BOARD_ASCLIN0_BAUDRATE 115200

#define BOARD_ISR_PRIORITY_ASCLIN0_RX 18
#define BOARD_ISR_PRIORITY_ASCLIN0_TX 19

#define BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_1KHz 6
#define BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_10KHz 7 // Highest number = hughest priority

#define BOARD_LED1_MODULE MODULE_P00
#define BOARD_LED1_PIN 5
#define BOARD_LED2_MODULE MODULE_P00
#define BOARD_LED2_PIN 6
#define BOARD_X2_3_MODULE MODULE_P00
#define BOARD_X2_3_PIN 0
#define BOARD_X2_5_MODULE MODULE_P00
#define BOARD_X2_5_PIN 2

#define BOARD_X2_9_MODULE MODULE_P00
#define BOARD_X2_9_PIN 8

#define BOARD_BTN1_MODULE MODULE_P00
#define BOARD_BTN1_PIN 7

#define BOARD_TIME_REF_MODULE MODULE_P00
#define BOARD_TIME_REF_PIN 1
#define BOARD_TASK_LOWFREQ_IO_MODULE BOARD_X2_3_MODULE
#define BOARD_TASK_LOWFREQ_IO_PIN BOARD_X2_3_PIN
#define BOARD_TASK_HIGHFREQ_IO_MODULE BOARD_X2_5_MODULE
#define BOARD_TASK_HIGHFREQ_IO_PIN BOARD_X2_5_PIN
#define BOARD_SCRUTINY_TRIGGER_MODULE BOARD_X2_9_MODULE
#define BOARD_SCRUTINY_TRIGGER_PIN BOARD_X2_9_PIN

#define STM_TARGET_FREQUENCY 20000000.0f // fsource = 300MHz. STMDIV = 15 (max) yields 20MHz.

#define BOARD_ISR_PRIORITY_ASCLIN1_RX 15
#define BOARD_ISR_PRIORITY_ASCLIN1_TX 16

#define BOARD_ASCLIN1_TX_BUFFER_SIZE 256
#define BOARD_ASCLIN1_RX_BUFFER_SIZE 128

#define BOARD_ASCLIN1_SERIAL_PIN_RX IfxAsclin1_RXA_P15_1_IN
#define BOARD_ASCLIN1_SERIAL_PIN_TX IfxAsclin1_TX_P15_1_OUT

extern IfxAsclin_Asc g_asclin0;
extern IfxAsclin_Asc g_asclin1;

void init_board();

void set_led1(bool val);
void set_led2(bool val);
void toggle_graph_trigger_pin();
void toggle_time_ref_pin();

inline uint32_t stm_timestamp()
{
    return IfxStm_getLower(&MODULE_STM0);
}

inline uint32_t stm_timestamp_diff_to_delta_100ns(uint32_t const timestamp_diff)
{
    // Assumes that the STM runs at 20MHz
    // Divide 20MHz in 2 to get 10MHz, which is 1/100ns
    return timestamp_diff >> 1;
}

#endif // DEMO_TC334_BOARD_HPP