
extern "C"
{
#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"
#include "IfxGpt12.h"
#include "IfxPort.h"
#include "Ifx_Types.h"
}

#include "board.hpp"
#include <cstdint>

IfxAsclin_Asc g_asclin0;

/* The transfer buffers allocate memory for the data itself and for FIFO runtime variables.
 8 more bytes have to be added to ensure a proper circular buffer handling independent from *the address to which the buffers have been located.
 */

uint8 g_asclin0_tx_buffer[BOARD_ASCLIN0_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
IFX_INTERRUPT(asclin0_Tx_ISR, 0, BOARD_ASCLIN0_INTPRIO_ASCLIN0_TX); // Interrupt service routine
IFX_INTERRUPT(interruptGpt12, 0, BOARD_ISR_PRIORITY_GPT12_TIMER);

void asclin0_Tx_ISR(void)
{
    IfxAsclin_Asc_isrTransmit(&g_asclin0);
}

void interruptGpt12(void)
{
    IfxPort_togglePin(&BOARD_LED1_PORT, BOARD_LED1_PIN); // Toggle LED state
}

void init_asclin0(void);
void initGpt12Timer(void);

void init_board()
{
    init_asclin0();
    initGpt12Timer();
}

// ==== ASCLIN 0 ====
void init_asclin0(void)
{
    IfxAsclin_Asc_Config ascConfig;
    IfxAsclin_Asc_initModuleConfig(&ascConfig, BOARD_ASCLIN0_SERIAL_PIN_TX.module);
    ascConfig.baudrate.baudrate = 115200;
    ascConfig.interrupt.txPriority = BOARD_ASCLIN0_INTPRIO_ASCLIN0_TX;
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
void initGpt12Timer(void)
{
    uint16_t RELOAD_VALUE = 48828u;
    // Initialize the GPT12 module
    IfxGpt12_enableModule(&MODULE_GPT120);                                          // Enable the GPT12 module
    IfxGpt12_setGpt1BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt1BlockPrescaler_16); // Set GPT1 block prescaler

    // Initialize the Timer T3
    IfxGpt12_T3_setMode(&MODULE_GPT120, IfxGpt12_Mode_timer);                       // Set T3 to timer mode
    IfxGpt12_T3_setTimerDirection(&MODULE_GPT120, IfxGpt12_TimerDirection_down);    // Set T3 count direction
    IfxGpt12_T3_setTimerPrescaler(&MODULE_GPT120, IfxGpt12_TimerInputPrescaler_64); // Set T3 input prescaler
    IfxGpt12_T3_setTimerValue(&MODULE_GPT120, RELOAD_VALUE);                        // Set T3 start value

    // Initialize the Timer T2
    IfxGpt12_T2_setMode(&MODULE_GPT120, IfxGpt12_Mode_reload);                               // Set T2 to reload mode
    IfxGpt12_T2_setReloadInputMode(&MODULE_GPT120, IfxGpt12_ReloadInputMode_bothEdgesTxOTL); // Set reload trigger
    IfxGpt12_T2_setTimerValue(&MODULE_GPT120, RELOAD_VALUE);                                 // Set T2 reload value

    // Initialize the interrupt
    volatile Ifx_SRC_SRCR *src = IfxGpt12_T3_getSrc(&MODULE_GPT120);                  // Get the interrupt address
    IfxSrc_init(src, BOARD_ISR_PROVIDER_GPT12_TIMER, BOARD_ISR_PRIORITY_GPT12_TIMER); // Initialize service request
    IfxSrc_enable(src);                                                               // Enable GPT12 interrupt

    // Initialize the LED
    IfxPort_setPinModeOutput(&BOARD_LED1_PORT, BOARD_LED1_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    IfxGpt12_T3_run(&MODULE_GPT120, IfxGpt12_TimerRun_start); // Start the timer
}
