#ifndef SCRUTINY_INTEGRATION_HPP
#define SCRUTINY_INTEGRATION_HPP

#include "scrutiny.hpp"
#include <cstdint>

extern "C"
{
#include "IfxAsclin_Asc.h"
}

void configure_scrutiny(IfxAsclin_Asc *const asclin);
void process_scrutiny_main(uint32_t const diff_time_us);

extern scrutiny::MainHandler scrutiny_main;
extern scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler;
extern scrutiny::FixedFrequencyLoopHandler task_1KHz_loop_handler;
extern scrutiny::FixedFrequencyLoopHandler task_10KHz_loop_handler;

#endif // SCRUTINY_INTEGRATION_HPP