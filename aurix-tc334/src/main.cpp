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
#include "IfxStm.h"
#include "board.hpp"
#include "scrutiny.hpp"
#include "scrutiny_integration.hpp"
#include "task_controller.hpp"
#include "wave_function_generator.hpp"
#include <cstdint>
#include <cstdio>

IFX_ALIGN(4) IfxCpu_syncEvent cpuSyncEvent = 0;

static WaveFunctionGenerator high_freq_func_gen(WaveFunctionGenerator::WaveType::SINE, 10, 0);
static WaveFunctionGenerator low_freq_func_gen(WaveFunctionGenerator::WaveType::TRIANGLE, 10, 0);
static WaveFunctionGenerator main_loop_func_gen(WaveFunctionGenerator::WaveType::SAWTOOTH, 10, 0);

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

    configure_scrutiny();
    TaskController::get_task_highfreq()->enable();
    TaskController::get_task_lowfreq()->enable();

    IfxCpu_enableInterrupts();

    uint32_t last_timestamp = stm_timestamp();

    high_freq_func_gen.set_phase(0);
    low_freq_func_gen.set_phase(0);
    main_loop_func_gen.set_phase(0);

    while (1)
    {
        // Overflow expected only if the task load is increased artificially by scrutiny
        bool const overflow = (TaskController::get_task_highfreq()->is_overflow() || TaskController::get_task_lowfreq()->is_overflow());
        set_led1(overflow);

        uint32_t timestamp = stm_timestamp();
        uint32_t const timediff_100ns = stm_timestamp_diff_to_delta_100ns(timestamp - last_timestamp);
        last_timestamp = timestamp;
        task_idle_loop_handler.process(timediff_100ns);
        main_loop_func_gen.update(static_cast<float>(timediff_100ns) * 1e-7f);
        process_scrutiny_main(timediff_100ns);
    }
}

void user_task_lowfreq()
{
    // Runs at 1KHz
    IfxPort_setPinHigh(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN);
    static uint32_t last_timestamp = stm_timestamp();

    // The load variable below simulate work in the scheduler task.
    // It can be modified at runtime by scrutiny. Takes ~6.73ns per unit.
    static volatile uint32_t lowfreq_load = 10000;
    for (uint32_t i = 0; i < lowfreq_load; i++)
        ;

    // Process scrutiny loop probe and measure the time precisely for it.
    uint32_t const timestamp = stm_timestamp();
    uint32_t const timediff_100ns = stm_timestamp_diff_to_delta_100ns(timestamp - last_timestamp);
    task_lowfreq_loop_handler.process(timediff_100ns);
    low_freq_func_gen.update(static_cast<float>(timediff_100ns) * 1e-7f);
    last_timestamp = timestamp;

    IfxPort_setPinLow(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN);
}

void user_task_highfreq()
{
    // Runs at 10KHz
    IfxPort_setPinHigh(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN);
    static uint32_t last_timestamp = stm_timestamp();

    // The load variable below simulate work in the scheduler task.
    // It can be modified at runtime by scrutiny. Takes ~6.73ns per unit.
    static volatile uint32_t highfreq_load = 1000; // Can be modified by scrutiny
    for (uint32_t i = 0; i < highfreq_load; i++)
        ;

    // Process scrutiny loop probe and measure the time precisely for it.
    uint32_t const timestamp = stm_timestamp();
    uint32_t const timediff_100ns = stm_timestamp_diff_to_delta_100ns(timestamp - last_timestamp);
    high_freq_func_gen.update(static_cast<float>(timediff_100ns) * 1e-7f);
    task_highfreq_loop_handler.process(timediff_100ns);
    last_timestamp = timestamp;

    IfxPort_setPinLow(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN);
}