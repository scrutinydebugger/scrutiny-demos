
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"
#include "scrutiny_integration.hpp"

#define DEBUG_PIN_1HZ_TOGGLE_PERIOD 5000000u // 500 msec in multiple of 100ns

extern bool scrutiny_init(uint32_t const sci_base);
extern void scrutiny_idle_update(uint32_t timediff_100ns);

inline uint32_t get_timestamp_100ns(void)
{
    // 100 MHZ / 10 = 100ns period
    // Timer count downward, ~ to invert and make it count up
    return ~CPUTimer_getTimerCount(CPUTIMER_TimeBase_BASE);
}

void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();
    Board_init();   // Invoke syscfg generated code
    C2000Ware_libraries_init(); // We don't use this. Let it in case

    if (scrutiny_init(XDS_SCIA_BASE) == false)
    {
        while(1);
    }

    EINT;
    ERTM;

    CPUTimer_startTimer(CPUTIMER_TimeBase_BASE);    // Free running counter, 1 tick per 100ns
    CPUTimer_startTimer(CPUTIMER_TASK_1K_BASE);     // Generate an interrupt every millisecond

    uint32_t last_timestamp = get_timestamp_100ns();
    uint32_t onesec_toggle_last_timestamp = get_timestamp_100ns();
    for(;;)
    {
        uint32_t const timestamp = get_timestamp_100ns();

        // Toggle a pin at a rate of 1Hz to validate the time base with a logic analyzer.
        if ( (timestamp - onesec_toggle_last_timestamp) >= DEBUG_PIN_1HZ_TOGGLE_PERIOD) // 500msec
        {
            GPIO_togglePin(GPIO_Idle1SecDebug); // J40 / Pin 33 (GPIO30)
            onesec_toggle_last_timestamp = timestamp;
        }

        scrutiny_idle_update(timestamp-last_timestamp);
        last_timestamp = timestamp;
    }
}

void INT_CPUTIMER_TASK_1K_ISR(void)
{
    // Toggle a pin to emasure the load and frequency of this task with a logic analyzer
    GPIO_writePin(GPIO_task1KDebug, 1); // J40 / Pin 34 (GPIO58)
    for (volatile int i=0; i<300; i++);
    task1KHz_LoopHandler.process();
    GPIO_writePin(GPIO_task1KDebug, 0);
}

