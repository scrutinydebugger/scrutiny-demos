//    scrutiny_integration.hpp
//        The app wrapper that integrates scrutiny in this demo by connecting the AscLin module
//        (UART) to the scrutiny streams.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

#ifndef SCRUTINY_INTEGRATION_HPP
#define SCRUTINY_INTEGRATION_HPP

#include "scrutiny.hpp"
#include <cstdint>

extern "C"
{
#include "IfxAsclin_Asc.h"
}

void configure_scrutiny();
void process_scrutiny_main(uint32_t const diff_time_us);

extern scrutiny::MainHandler scrutiny_main;
extern scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler;
extern scrutiny::FixedFrequencyLoopHandler task_lowfreq_loop_handler;
extern scrutiny::FixedFrequencyLoopHandler task_highfreq_loop_handler;

#endif // SCRUTINY_INTEGRATION_HPP