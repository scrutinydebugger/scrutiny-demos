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

static volatile bool sync_all_wavegen = false;
static volatile bool btn1_pressed;

/// @brief Startup the CPU. Following Infineon examples to startup the core.
void setup_cpu()
{
    IfxCpu_enableInterrupts();

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Cpu sync event wait
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);
}

extern "C" void core0_main(void)
{
    setup_cpu();  // Boot the core
    init_board(); // Configure peripherals

    configure_scrutiny(); // Configure the Scrutiny Embedded lib

    // enable 10KHz & 1KHz task with little home made task scheduler based on GPT12 timer interrupts
    TaskController::get_task_highfreq()->enable();
    TaskController::get_task_lowfreq()->enable();

    IfxCpu_enableInterrupts();

    uint32_t last_timestamp = stm_timestamp(); // Will use that timestamp to track time
    sync_all_wavegen = true;

    while (true)
    {
        uint32_t const timestamp = stm_timestamp();

        // Overflow expected only if the task load is increased artificially by scrutiny
        bool const overflow = (TaskController::get_task_highfreq()->is_overflow() || TaskController::get_task_lowfreq()->is_overflow());
        set_led1(overflow);

        if (sync_all_wavegen) // Controlled by scrutiny
        {
            main_loop_func_gen.set_phase(0);
            low_freq_func_gen.set_phase(0);
            high_freq_func_gen.set_phase(0);
            sync_all_wavegen = false;
        }

        // We could use this button to trigger a scrutiny graph
        btn1_pressed = !IfxPort_getPinState(&BOARD_BTN1_MODULE, BOARD_BTN1_PIN); // Active low

        // Measure the time increase and update Scrutiny and our demo wave generator
        uint32_t const timediff_100ns = stm_timestamp_diff_to_delta_100ns(timestamp - last_timestamp);
        last_timestamp = timestamp;
        task_idle_loop_handler.process(timediff_100ns);
        main_loop_func_gen.update(static_cast<float>(timediff_100ns) * 1e-7f);
        process_scrutiny_main(timediff_100ns);
    }
}

/// @brief Low frequency task (1KHz) with low priority controlled by interrupt with a minimalistic custom made task scheduler
void user_task_lowfreq()
{
    // Runs at 1KHz. Cannot interrupt the 10KHz task
    IfxPort_setPinHigh(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN); // Debug pin for logic analyzer
    static uint32_t last_timestamp = stm_timestamp();
    uint32_t const timestamp = stm_timestamp();

    // The load variable below simulate work in the scheduler task.
    // It can be modified at runtime by scrutiny. Takes ~6.73ns per unit.
    static volatile uint32_t lowfreq_load = 10000;
    for (uint32_t i = 0; i < lowfreq_load; i++)
        ;

    // Process scrutiny loop probe and measure the time precisely for it.
    uint32_t const timediff_100ns = stm_timestamp_diff_to_delta_100ns(timestamp - last_timestamp);
    task_lowfreq_loop_handler.process(timediff_100ns);
    low_freq_func_gen.update(static_cast<float>(timediff_100ns) * 1e-7f);
    last_timestamp = timestamp;

    IfxPort_setPinLow(&BOARD_TASK_LOWFREQ_IO_MODULE, BOARD_TASK_LOWFREQ_IO_PIN); // Debug pin for logic analyzer
}

/// @brief High frequency task (10KHz) with high priority controlled by interrupt with a minimalistic custom made task scheduler
void user_task_highfreq()
{
    // Runs at 10KHz. Can interrupt the 1KHz task
    IfxPort_setPinHigh(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN); // Debug pin for logic analyzer
    static uint32_t last_timestamp = stm_timestamp();
    uint32_t const timestamp = stm_timestamp();

    // The load variable below simulate work in the scheduler task.
    // It can be modified at runtime by scrutiny. Takes ~6.73ns per unit.
    static volatile uint32_t highfreq_load = 1000; // Can be modified by scrutiny
    for (uint32_t i = 0; i < highfreq_load; i++)
        ;

    // Process scrutiny loop probe and measure the time precisely for it.
    uint32_t const timediff_100ns = stm_timestamp_diff_to_delta_100ns(timestamp - last_timestamp);
    high_freq_func_gen.update(static_cast<float>(timediff_100ns) * 1e-7f);
    task_highfreq_loop_handler.process(timediff_100ns);
    last_timestamp = timestamp;

    IfxPort_setPinLow(&BOARD_TASK_HIGHFREQ_IO_MODULE, BOARD_TASK_HIGHFREQ_IO_PIN); // Debug pin for logic analyzer
}