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

/* The transfer buffers allocate memory for the data itself and for FIFO runtime variables.
 8 more bytes have to be added to ensure a proper circular buffer handling independent from *the address to which the buffers have been located.
 */

uint8 g_asclin0_tx_buffer[BOARD_ASCLIN0_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

IFX_INTERRUPT(asclin0_Tx_ISR, 0, BOARD_ISR_PRIORITY_ASCLIN0_TX);
IFX_INTERRUPT(interrupt_gpt12_T2, 0, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_10KHz);
IFX_INTERRUPT(interrupt_gpt12_T4, 0, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_1KHz);

static constexpr uint16_t TIMER2_VAL = 2500;
static constexpr uint16_t TIMER4_VAL = 6250;

void asclin0_Tx_ISR(void)
{
    IfxAsclin_Asc_isrTransmit(&g_asclin0);
}

void task_1khz();
void task_10khz();

void init_io(void);
void init_asclin0(void);
void init_gpt12(void);

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
    init_gpt12();

    set_led1(false);
    set_led2(false);
}

void init_io(void)
{
    IfxPort_setPinModeOutput(&BOARD_LED1_MODULE, BOARD_LED1_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_LED2_MODULE, BOARD_LED2_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
}

// ==== ASCLIN 0 ====
void init_asclin0(void)
{
    IfxAsclin_Asc_Config ascConfig;
    IfxAsclin_Asc_initModuleConfig(&ascConfig, BOARD_ASCLIN0_SERIAL_PIN_TX.module);
    ascConfig.baudrate.baudrate = 115200;
    ascConfig.interrupt.txPriority = BOARD_ISR_PRIORITY_ASCLIN0_TX;
    ascConfig.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    ascConfig.txBuffer = &g_asclin0_tx_buffer;
    ascConfig.txBufferSize = BOARD_ASCLIN0_TX_BUFFER_SIZE;

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
    ascConfig.pins = &pins;

    IfxAsclin_Asc_initModule(&g_asclin0, &ascConfig);
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
    IfxSrc_init(t2_src, BOARD_ISR_PROVIDER_GPT12_TIMER, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_10KHz);
    IfxSrc_enable(t2_src);

    volatile Ifx_SRC_SRCR *t4_src = IfxGpt12_T4_getSrc(&MODULE_GPT120);
    IfxSrc_init(t4_src, BOARD_ISR_PROVIDER_GPT12_TIMER, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_1KHz);
    IfxSrc_enable(t4_src);

    IfxGpt12_T2_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
    IfxGpt12_T4_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
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
