#include "application.hpp"

#include "SEGGER_RTT.h"
#include "scrutiny.hpp"
#include "main.h"
#include "stm32f4xx_hal.h"

Application app;

// C/C++ glue code
extern "C" void init_app(void)
{
	app.init();
}

extern "C" int run_app(void)
{
	return app.run();
}

extern "C" void app_tim3_interrupt(void)
{
	app.task_1khz();
}


Application::Application(void) :
	m_scrutiny_integration(0)
{

}

void Application::init(void)
{
	SEGGER_RTT_Init();
	m_scrutiny_integration.init();
}

int Application::run(void)
{
	init();
	uint32_t last_tick = tick_1us();
	uint32_t wave_1hz_last_tick = tick_1us();
	while(true)
	{
		uint32_t const tick = tick_1us();
		uint32_t timediff_us = tick - last_tick;

		if (tick - wave_1hz_last_tick >= 500 * 1e3 - 1)	// 500 ms
		{
			HAL_GPIO_TogglePin(Square_1Hz_GPIO_Port, Square_1Hz_Pin);
			HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
			wave_1hz_last_tick = tick;
		}

		m_scrutiny_integration.update(timediff_us * 10);
		last_tick = tick;
	}
	return 0;
}



void Application::task_1khz(void)
{
	static uint32_t last_tick_us = tick_1us();

	uint32_t const tick_us = tick_1us();
	uint32_t const timediff_us = (tick_us - last_tick_us);

	m_scrutiny_integration.loop_1khz_exec(timediff_us * 10);
	
	last_tick_us = tick_us;
}
