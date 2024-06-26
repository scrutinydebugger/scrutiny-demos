//    scrutiny_integration.hpp
//        Glue code between the scrutiny lib and the application
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2024 Scrutiny Debugger

#ifndef SCRUTINY_INTEGRATION_HPP
#define SCRUTINY_INTEGRATION_HPP

#include <stdint.h>
#include "scrutiny.hpp"

void nsec2024_demo_config_scrutiny();
void nsec2024_demo_update_scrutiny_main(uint32_t const diff_time_us);

extern scrutiny::MainHandler scrutiny_main;
extern scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler;
extern scrutiny::FixedFrequencyLoopHandler task_100hz_loop_handler; 
extern scrutiny::FixedFrequencyLoopHandler task_20hz_loop_handler; 

#endif