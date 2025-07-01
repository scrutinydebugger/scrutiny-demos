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
#include "scrutiny_integration.hpp"

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

    while (1)
    {
        process_scrutiny_main(0);
    }
}
