//    main.cpp
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

extern "C"
{
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Cfg_Ssw.h"
#include "Ifx_Types.h"
}

// #include "board.hpp"
#include "board.hpp"
#include "scrutiny.hpp"
#include "scrutiny_integration.hpp"
#include "task_controller.hpp"

#include <cstdint>

IFX_ALIGN(4) IfxCpu_syncEvent cpuSyncEvent = 0;

void setup_cpu()
{
    IfxCpu_enableInterrupts();
    /*
     * !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Cpu sync event wait*/
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);
}

extern "C"
#if !defined(IFX_CFG_SSW_RETURN_FROM_MAIN)
    void
    core0_main(void)
{
#else
    int
    core0_main(void)
{
#endif
    setup_cpu();
    init_board();

    configure_scrutiny(&g_asclin0);
    TaskController::get_task_highfreq()->enable();
    TaskController::get_task_lowfreq()->enable();

    while (1)
    {
        // Overflow expected only if the task load is increased artificially by scrutiny
        bool const overflow = (TaskController::get_task_highfreq()->is_overflow() || TaskController::get_task_lowfreq()->is_overflow());
        set_led1(overflow);

        // process_scrutiny_main(0);
    }
}

void user_task_lowfreq()
{
    // Runs at 1KHz
    IfxPort_setPinHigh(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN);

    // The load variable below simulate work in the scheduler task.
    // It can be modified at runtime by scrutiny. Takes ~6.73ns per unit.
    static volatile uint32_t lowfreq_load = 10000;
    for (uint32_t i = 0; i < lowfreq_load; i++)
        ;
    IfxPort_setPinLow(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN);
    // task_lowfreq_loop_handler.process();
}

void user_task_highfreq()
{
    // Runs at 10KHz
    IfxPort_setPinHigh(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN);

    // The load variable below simulate work in the scheduler task.
    // It can be modified at runtime by scrutiny. Takes ~6.73ns per unit.
    static volatile uint32_t highfreq_load = 1000; // Can be modified by scrutiny
    for (uint32_t i = 0; i < highfreq_load; i++)
        ;
    IfxPort_setPinLow(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN);
    // task_highfreq_loop_handler.process();
}