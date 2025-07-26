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
#include "IfxCan_Can.h"
#include "IfxCan.h"
}

#include "board.hpp"
#include "task_controller.hpp"
#include "scrutiny_integration.hpp"
#include <cstdint>

IfxAsclin_Asc g_asclin0;
IfxAsclin_Asc g_asclin1;
IfxCan_Can    g_mcmcan0; 
IfxCan_Can_Node g_can_node0; 

// Define buffer size based on Infineon UART examples
uint8 g_asclin0_tx_buffer[BOARD_ASCLIN0_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
uint8 g_asclin0_rx_buffer[BOARD_ASCLIN0_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
uint8 g_asclin1_tx_buffer[BOARD_ASCLIN1_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
uint8 g_asclin1_rx_buffer[BOARD_ASCLIN1_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

uint8 s_scrutiny_rx_buffer[BOARD_CAN_SCRUTINY_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
Ifx_Fifo* g_scrutiny_can_rx_fifo = nullptr;

IFX_INTERRUPT(interrupt_asclin0_tx, 0, BOARD_ISR_PRIORITY_ASCLIN0_TX);
IFX_INTERRUPT(interrupt_asclin0_rx, 0, BOARD_ISR_PRIORITY_ASCLIN0_RX);
IFX_INTERRUPT(interrupt_asclin1_tx, 0, BOARD_ISR_PRIORITY_ASCLIN1_TX);
IFX_INTERRUPT(interrupt_asclin1_rx, 0, BOARD_ISR_PRIORITY_ASCLIN1_RX);
IFX_INTERRUPT(interrupt_gpt12_T3, 0, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_10KHz);
IFX_INTERRUPT(interrupt_gpt12_T6, 0, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_1KHz);
IFX_INTERRUPT(interrupt_can_rx, 0, BOARD_ISR_PRIORITY_CAN_RX);

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

void interrupt_can_rx(void)
{
    static int interrupt_count=0;
    interrupt_count++;
    static IfxCan_Message msg;
    static Ifx_SizeT const dlc_size_lut[16] = {0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64};
    union{
        uint8_t data8[8];
        uint32_t data32[2];
    } data;

    IfxCan_Node_clearInterruptFlag(g_can_node0.node, IfxCan_Interrupt_rxFifo0NewMessage);
    msg.readFromRxFifo0 = TRUE;
    
    IfxCan_Can_readMessage(&g_can_node0, &msg, data.data32);
    
    Ifx_SizeT const nb_bytes = dlc_size_lut[static_cast<int>(msg.dataLengthCode)];
    
    if (msg.messageId == SCRUTINY_CAN_RX_ID && msg.messageIdLength == IfxCan_MessageIdLength_standard)
    {
        Ifx_Fifo_write(g_scrutiny_can_rx_fifo, &data.data8, nb_bytes, 0 );  
    }
    else
    {
        // Ignore other messages if any
    }
}


void task_1khz();
void task_10khz();

void init_io(void);
void init_asclin0(void);
void init_asclin1(void);
void init_mcmcan();
void init_gpt12(void);
void init_stm(void);

void interrupt_gpt12_T3(void)
{
    TaskController::Task *const task = TaskController::get_task_highfreq();
    volatile Ifx_SRC_SRCR *src = IfxGpt12_T3_getSrc(&MODULE_GPT120);
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

void interrupt_gpt12_T6(void)
{
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
    set_led1(false);
    set_led2(false);
    init_asclin0();
    init_asclin1();
    init_mcmcan();
    init_gpt12();
    init_stm();

}

void init_io(void)
{
    IfxPort_setPinModeOutput(&BOARD_LED1_MODULE, BOARD_LED1_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_LED2_MODULE, BOARD_LED2_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_SCRUTINY_TRIGGER_MODULE, BOARD_SCRUTINY_TRIGGER_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_TIME_REF_MODULE, BOARD_TIME_REF_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&BOARD_CAN_STANDBY_MODULE, BOARD_CAN_STANDBY_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

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
        NULL_PTR,                              // RX Pin not used
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

    constexpr uint16_t TIMER3_VAL = 2500;
    constexpr uint16_t TIMER6_VAL = 6250;

    // Timer T3 - Task 10KHz : 100e6/4/1/2500 = 10000
    IfxGpt12_T3_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);
    IfxGpt12_T3_setTimerDirection(&MODULE_GPT120, IfxGpt12_TimerDirection_down);
    IfxGpt12_T3_setTimerPrescaler(&MODULE_GPT120, IfxGpt12_TimerInputPrescaler_1); // Prescaler for T3 only
    IfxGpt12_T3_setTimerValue(&MODULE_GPT120, TIMER3_VAL);                         // Set at start

    // Timer T2 is not counting. it simply store the reload value of T3
    IfxGpt12_T2_setMode(&MODULE_GPT120, IfxGpt12_Mode_reload);
    IfxGpt12_T2_setReloadInputMode(&MODULE_GPT120, IfxGpt12_ReloadInputMode_bothEdgesTxOTL);
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, TIMER3_VAL);

    // Timer T6 - Task 1KHz : 100e6/16/1/6250 = 1000
    IfxGpt12_setGpt2BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt2BlockPrescaler_16);
    IfxGpt12_T6_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);
    IfxGpt12_T6_setTimerDirection(&MODULE_GPT120, IfxGpt12_TimerDirection_down);
    IfxGpt12_T6_setReloadMode(&MODULE_GPT120, IfxGpt12_TimerReloadMode_enable); // will use CAPREL on  underflow
    IfxGpt12_T6_setTimerPrescaler(&MODULE_GPT120, IfxGpt12_TimerInputPrescaler_1);
    IfxGpt12_T6_setTimerValue(&MODULE_GPT120, TIMER6_VAL);

    IfxGpt12_setCaptureReload(&MODULE_GPT120, TIMER6_VAL); // Value reloaded by T6 on underflow

    // Initialize the interrupt
    volatile Ifx_SRC_SRCR *t3_src = IfxGpt12_T3_getSrc(&MODULE_GPT120);
    IfxSrc_init(t3_src, IfxCpu_Irq_getTos(IfxCpu_getCoreIndex()), BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_10KHz);
    IfxSrc_enable(t3_src);

    volatile Ifx_SRC_SRCR *t6_src = IfxGpt12_T6_getSrc(&MODULE_GPT120);
    IfxSrc_init(t6_src, IfxSrc_Tos_cpu0, BOARD_ISR_PRIORITY_GPT12_TIMER_TASK_1KHz);
    IfxSrc_enable(t6_src);

    IfxGpt12_T3_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
    IfxGpt12_T6_run(&MODULE_GPT120, IfxGpt12_TimerRun_start);
}

void init_stm(void)
{
    // We use this timer as a time reference for updating scrutiny
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
    // Trigger a pin when the scrutiny datalogger trigger condition is met for the required hold time
    IfxPort_togglePin(&BOARD_SCRUTINY_TRIGGER_MODULE, BOARD_SCRUTINY_TRIGGER_PIN);
}

void toggle_time_ref_pin()
{
    // Trigger a pin periodically to validate the time reference with a logic analyzer
    IfxPort_togglePin(&BOARD_TIME_REF_MODULE, BOARD_TIME_REF_PIN);
}


void init_mcmcan(void)
{
    static IfxCan_Can_Config canConfig;
    static IfxCan_Can_NodeConfig canNodeConfig;

    g_scrutiny_can_rx_fifo = Ifx_Fifo_init(s_scrutiny_rx_buffer, BOARD_CAN_SCRUTINY_RX_BUFFER_SIZE, 1);

    IfxCan_Can_initModuleConfig(&canConfig, &MODULE_CAN0);
    IfxCan_Can_initModule(&g_mcmcan0, &canConfig);

    IfxCan_Can_initNodeConfig(&canNodeConfig, &g_mcmcan0);

    canNodeConfig.nodeId = IfxCan_NodeId_0;
    canNodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;
    canNodeConfig.rxConfig.rxMode = IfxCan_RxMode_fifo0;
    canNodeConfig.rxConfig.rxFifo0DataFieldSize = IfxCan_DataFieldSize_8;
    canNodeConfig.rxConfig.rxFifo0OperatingMode = IfxCan_RxFifoMode_overwrite;
    canNodeConfig.rxConfig.rxFifo0Size = 4; // No need to use a lot, we have an interrupt on rx. 1 would be enough
    
    canNodeConfig.txConfig.txMode = IfxCan_TxMode_fifo;
    canNodeConfig.txConfig.txFifoQueueSize = 32;
    canNodeConfig.txConfig.txBufferDataFieldSize = IfxCan_DataFieldSize_8;

    canNodeConfig.interruptConfig.rxFifo0NewMessageEnabled = TRUE;
    canNodeConfig.interruptConfig.rxf0n.priority = BOARD_ISR_PRIORITY_CAN_RX;
    canNodeConfig.interruptConfig.rxf0n.interruptLine = IfxCan_InterruptLine_1;
    canNodeConfig.interruptConfig.rxf0n.typeOfService = IfxSrc_Tos_cpu0;

    canNodeConfig.baudRate.baudrate = 500000;
    canNodeConfig.fastBaudRate.baudrate = 500000;

    const IfxCan_Can_Pins pins = {
        &IfxCan_TXD00_P20_8_OUT,                //txPin
        IfxPort_OutputMode_pushPull,            //txPinMode
        &IfxCan_RXD00B_P20_7_IN,                //rxPin
        IfxPort_InputMode_noPullDevice,         //rxPinMode
        IfxPort_PadDriver_cmosAutomotiveSpeed1  //padDriver
    };

    canNodeConfig.pins = &pins;
    
    IfxCan_Can_initNode(&g_can_node0, &canNodeConfig);

    IfxPort_setPinLow(&BOARD_CAN_STANDBY_MODULE, BOARD_CAN_STANDBY_PIN);    // Enable the CAN transceiver by setting CAN_STB to low

}

//IfxCan_RXD00B_P20_7_IN
//IfxCan_TXD00_P20_8_OUT