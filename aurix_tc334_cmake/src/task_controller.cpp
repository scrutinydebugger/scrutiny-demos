//    task_controller.cpp
//        A lightweight task scheduler that abstract the timers
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

void user_task_highfreq();
void user_task_lowfreq();

TaskController::Task TaskController::s_task_lowfreq(user_task_lowfreq);
TaskController::Task TaskController::s_task_highfreq(user_task_highfreq);

void TaskController::exec_task(Task *const task)
{
    if (task->m_running)
    {
        task->triger_overflow();
    }

    if (!task->enabled())
    {
        return;
    }

    task->m_running = true;
    task->m_callback();
    task->m_running = false;
}
