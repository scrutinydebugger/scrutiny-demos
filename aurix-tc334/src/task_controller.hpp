//    task_controller.hpp
//        A lightweight task scheduler that abstract the timers
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

#ifndef TASK_CONTROLLER_HPP
#define TASK_CONTROLLER_HPP

#include <cstdint>

class TaskController
{
  public:
    class Task
    {
        friend class TaskController;

      public:
        Task(void (*callback)(void)) :
            m_running(false),
            m_overflow(false),
            m_enable(false),
            m_callback(callback)
        {
        }

        inline void triger_overflow()
        {
            m_overflow = true;
            m_enable = false;
        };

        inline void enable() { m_enable = true; };
        inline void disable() { m_enable = false; };
        inline bool enabled() const { return m_enable; };
        inline bool is_overflow() const { return m_overflow; };

      private:
        volatile bool m_running;
        volatile bool m_overflow;
        volatile bool m_enable;
        void (*m_callback)(void);
    };

    static void exec_task(Task *const task);

    static inline Task *get_task_highfreq(void) { return &s_task_highfreq; }
    static inline Task *get_task_lowfreq(void) { return &s_task_lowfreq; }

  private:
    static Task s_task_lowfreq;
    static Task s_task_highfreq;
};

#endif