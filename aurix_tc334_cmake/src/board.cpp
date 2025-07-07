//    board.cpp
//        The abstraction layer to configure the dev board (kit_a2g_tc334_lite)
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

extern "C"
{
#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"
#include "IfxGpt12.h"
#include "IfxPort.h"
#include "Ifx_Types.h"
}

#include "board.hpp"
#include "task_controller.hpp"
#include <cstdint>

IfxAsclin_Asc g_asclin0;
IfxAsclin_Asc g_asclin1;

// Define buffer size based on Infineon UART examples
uint8 g_asclin0_tx_buffer[BOARD_ASCLIN0_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
uint8 g_asclin0_rx_buffer[BOARD_ASCLIN0_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
uint8 g_asclin1_tx_buffer[BOARD_ASCLIN1_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
uint8 g_asclin1_rx_buffer[BOARD_ASCLIN1_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

IFX_INTERRUPT(interrupt_asclin0_tx, 0, BOARD_ISR_PRIORITY_ASCLIN0_TX);
IFX_INTERRUPT(interrupt_asclin0_rx, 0, BOARD_ISR_PRIORITY_ASCLIN0_RX);
IFX_INTERRUPT(interrupt_asclin1_tx, 0, BOARD_ISR_PRIORITY_ASCLIN1_TX);
IFX_INTERRUPT(interrupt_asclin1_rx, 0, BOARD_ISR_PRIORITY_ASCLIN1_RX);
IFX_INTERRUPT(interrupt_gpt12_T2, 0, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_10KHz);
IFX_INTERRUPT(interrupt_gpt12_T4, 0, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_1KHz);

static constexpr uint16_t TIMER2_VAL = 2500;
static constexpr uint16_t TIMER4_VAL = 6250;

void interrupt_asclin0_tx(void)
{
    IfxAsclin_Asc_isrTransmit(&g_asclin0);
}

void interrupt_asclin0_rx(void)
{
    IfxAsclin_Asc_isrReceive(&g_asclin0);
}

void interrupt_asclin1_tx(void)
{
    IfxAsclin_Asc_isrTransmit(&g_asclin1);
}

void interrupt_asclin1_rx(void)
{
    IfxAsclin_Asc_isrReceive(&g_asclin1);
}

void task_1khz();
void task_10khz();

void init_io(void);
void init_asclin0(void);
void init_asclin1(void);
void init_gpt12(void);
void init_stm(void);

void interrupt_gpt12_T2(void)
{
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, TIMER2_VAL);
    TaskController::Task *const task = TaskController::get_task_highfreq();
    volatile Ifx_SRC_SRCR *src = IfxGpt12_T2_getSrc(&MODULE_GPT120);
    if (src->B.IOV)
    {
        task->triger_overflow();
    }
    else
    {
        __bisr(BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_10KHz - 1); // Allow reentry
        TaskController::exec_task(task);
        _rslcx(); // required because of __bisr
    }
}

void interrupt_gpt12_T4(void)
{
    IfxGpt12_T4_setTimerValue(&MODULE_GPT120, TIMER4_VAL);
    TaskController::Task *const task = TaskController::get_task_lowfreq();
    volatile Ifx_SRC_SRCR *src = IfxGpt12_T4_getSrc(&MODULE_GPT120);
    if (src->B.IOV)
    {
        task->triger_overflow();
    }
    else
    {
        __bisr(BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_1KHz - 1); // Allow reentry
        TaskController::exec_task(task);
        __rslcx(); // required because of __bisr
    }
}

void init_board()
{
    init_io();
    init_asclin0();
    init_asclin1();
    init_gpt12();
    init_stm();

    set_led1(false);
    set_led2(false);
}

void init_io(void)
{
    IfxPort_setPinModeOutput(&BOARD_LED1_MODULE, BOARD_LED1_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_LED2_MODULE, BOARD_LED2_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_SCRUTINY_TRIGGER_MODULE, BOARD_SCRUTINY_TRIGGER_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_TIME_REF_MODULE, BOARD_TIME_REF_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    IfxPort_setPinModeInput(&BOARD_BTN1_MODULE, BOARD_BTN1_PIN, IfxPort_InputMode_noPullDevice);
}

// ==== ASCLIN 0 ====
void init_asclin0(void)
{
    IfxAsclin_Asc_Config config;
    IfxAsclin_Asc_initModuleConfig(&config, &MODULE_ASCLIN0);
    config.baudrate.baudrate = 115200;

    // Default of SamplePointPosition_3 with oversamp = 1 cause issues in receiving at 115200.
    // Below configuration is reliable and taken from code example that receives data over UART (no just transmit).
    config.baudrate.oversampling = IfxAsclin_OversamplingFactor_16;
    config.bitTiming.medianFilter = IfxAsclin_SamplesPerBit_three;
    config.bitTiming.samplePointPosition = IfxAsclin_SamplePointPosition_8;

    config.interrupt.txPriority = BOARD_ISR_PRIORITY_ASCLIN0_TX;
    config.interrupt.rxPriority = BOARD_ISR_PRIORITY_ASCLIN0_RX;
    config.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    config.frame.parityBit = false;
    config.frame.stopBit = IfxAsclin_StopBit_1;

    config.txBuffer = &g_asclin0_tx_buffer;
    config.txBufferSize = BOARD_ASCLIN0_TX_BUFFER_SIZE;

    config.rxBuffer = &g_asclin0_rx_buffer;
    config.rxBufferSize = BOARD_ASCLIN0_RX_BUFFER_SIZE;

    const IfxAsclin_Asc_Pins pins = {
        NULL_PTR,                              // CTS pin not used
        IfxPort_InputMode_pullUp,              // CTS pin pullup
        &BOARD_ASCLIN0_SERIAL_PIN_RX,          // RX Pin
        IfxPort_InputMode_pullUp,              // RX pin pullup
        NULL_PTR,                              // RTS pin not used
        IfxPort_OutputMode_pushPull,           // RTS pin pushpull
        &BOARD_ASCLIN0_SERIAL_PIN_TX,          // TX pin
        IfxPort_OutputMode_pushPull,           // TX pin pushpull
        IfxPort_PadDriver_cmosAutomotiveSpeed1 // Pin driver
    };
    config.pins = &pins;

    IfxAsclin_Asc_initModule(&g_asclin0, &config);
}

void init_asclin1(void)
{
    IfxAsclin_Asc_Config config;
    IfxAsclin_Asc_initModuleConfig(&config, &MODULE_ASCLIN1);
    config.baudrate.baudrate = 115200;

    // Default of SamplePointPosition_3 with oversamp = 1 cause issues in receiving at 115200.
    // Below configuration is reliable and taken from code example that receives data over UART (no just transmit).
    config.baudrate.oversampling = IfxAsclin_OversamplingFactor_16;
    config.bitTiming.medianFilter = IfxAsclin_SamplesPerBit_three;
    config.bitTiming.samplePointPosition = IfxAsclin_SamplePointPosition_8;

    config.interrupt.txPriority = BOARD_ISR_PRIORITY_ASCLIN1_TX;
    config.interrupt.rxPriority = BOARD_ISR_PRIORITY_ASCLIN1_RX;
    config.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    config.frame.parityBit = false;
    config.frame.stopBit = IfxAsclin_StopBit_1;

    config.txBuffer = &g_asclin1_tx_buffer;
    config.txBufferSize = BOARD_ASCLIN1_TX_BUFFER_SIZE;

    config.rxBuffer = &g_asclin1_rx_buffer;
    config.rxBufferSize = BOARD_ASCLIN1_RX_BUFFER_SIZE;

    const IfxAsclin_Asc_Pins pins = {
        NULL_PTR,                              // CTS pin not used
        IfxPort_InputMode_pullUp,              // CTS pin pullup
        &NULL_PTR,                             // RX Pin not used
        IfxPort_InputMode_pullUp,              // RX pin pullup
        NULL_PTR,                              // RTS pin not used
        IfxPort_OutputMode_pushPull,           // RTS pin pushpull
        &BOARD_ASCLIN1_SERIAL_PIN_TX,          // TX pin
        IfxPort_OutputMode_pushPull,           // TX pin pushpull
        IfxPort_PadDriver_cmosAutomotiveSpeed1 // Pin driver
    };
    config.pins = &pins;

    IfxAsclin_Asc_initModule(&g_asclin1, &config);
}

// ===== GPT12 ======
void init_gpt12(void)
{
    IfxGpt12_enableModule(&MODULE_GPT120);
    IfxGpt12_setGpt1BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt1BlockPrescaler_4); // Prescaler for T2, T3, T4

    // Timer T2 - Task 10KHz : 100e6/4/1/2500 = 10000
    IfxGpt12_T2_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);
    IfxGpt12_T2_setTimerDirection(&MODULE_GPT120, IfxGpt12_TimerDirection_down);
    IfxGpt12_T2_setTimerPrescaler(&MODULE_GPT120, IfxGpt12_TimerInputPrescaler_1); // Prescaler for T2 only
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, TIMER2_VAL);

    // Timer T4 - Task 1KHz : 100e6/4/4/6250 = 1000
    IfxGpt12_T4_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);
    IfxGpt12_T4_setTimerDirection(&MODULE_GPT120, IfxGpt12_TimerDirection_down);
    IfxGpt12_T4_setTimerPrescaler(&MODULE_GPT120, IfxGpt12_TimerInputPrescaler_4); // Prescaler for T4 only
    IfxGpt12_T4_setTimerValue(&MODULE_GPT120, TIMER4_VAL);

    // Initialize the interrupt
    volatile Ifx_SRC_SRCR *t2_src = IfxGpt12_T2_getSrc(&MODULE_GPT120);
    IfxSrc_init(t2_src, IfxCpu_Irq_getTos(IfxCpu_getCoreIndex()), BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_10KHz);
    IfxSrc_enable(t2_src);

    volatile Ifx_SRC_SRCR *t4_src = IfxGpt12_T4_getSrc(&MODULE_GPT120);
    IfxSrc_init(t4_src, IfxCpu_Irq_getTos(IfxCpu_getCoreIndex()), BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_1KHz);
    IfxSrc_enable(t4_src);

    IfxGpt12_T2_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
    IfxGpt12_T4_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
}

void init_stm(void)
{
    IfxScuCcu_setStmFrequency(STM_TARGET_FREQUENCY);
}

void set_led1(bool val)
{
    if (val)
    {
        IfxPort_setPinLow(&BOARD_LED1_MODULE, BOARD_LED1_PIN);
    }
    else
    {
        IfxPort_setPinHigh(&BOARD_LED1_MODULE, BOARD_LED1_PIN);
    }
}

void set_led2(bool val)
{
    if (val)
    {
        IfxPort_setPinLow(&BOARD_LED2_MODULE, BOARD_LED2_PIN);
    }
    else
    {
        IfxPort_setPinHigh(&BOARD_LED2_MODULE, BOARD_LED2_PIN);
    }
}

void toggle_graph_trigger_pin()
{
    IfxPort_togglePin(&BOARD_SCRUTINY_TRIGGER_MODULE, BOARD_SCRUTINY_TRIGGER_PIN);
}

void toggle_time_ref_pin()
{
    IfxPort_togglePin(&BOARD_TIME_REF_MODULE, BOARD_TIME_REF_PIN);
}