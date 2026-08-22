//    application.cpp
//        The high level application invoked from main
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//    Copyright (c) 2026 Scrutiny Debugger

#include "application.hpp"

#include "SEGGER_RTT.h"
#include "scrutiny.hpp"
#include "main.h"
#include "stm32f4xx_hal.h"

#include <algorithm>
#include <cstring>

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
	m_scrutiny_integration(0),
	m_main_loop_generator(WaveFunctionGenerator::WaveType::SINE, 10, 0),
	m_isr_1khz_generator(WaveFunctionGenerator::WaveType::SINE, 10, 0)

{

}

float Application::s_requested_led_freq = 1.0f;

void Application::init(void)
{
	SEGGER_RTT_Init();
	m_scrutiny_integration.init(scrutiny_user_command_callback);
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

		uint32_t const led_max_count = static_cast<uint32_t>(1000000.0f / s_requested_led_freq / 2.0f);
		if ((tick - wave_1hz_last_tick) >= led_max_count - 1)
		{
			HAL_GPIO_TogglePin(Square_1Hz_GPIO_Port, Square_1Hz_Pin);
			HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
			wave_1hz_last_tick = tick;
		}

        if (m_sync_all_wavegen) // Controlled by scrutiny
        {
        	m_main_loop_generator.set_phase(0);
        	m_isr_1khz_generator.set_phase(0);
            m_sync_all_wavegen = false;
        }

		m_user_key_pressed = HAL_GPIO_ReadPin(BTN_KEY_GPIO_Port, BTN_KEY_Pin) == GPIO_PIN_RESET;
		m_main_loop_generator.update(static_cast<float>(timediff_us) / 1000000.0f);
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

	m_isr_1khz_generator.update(static_cast<float>(timediff_us) / 1000000.0f);
	m_scrutiny_integration.loop_1khz_exec(timediff_us * 10);
	
	last_tick_us = tick_us;
}


void Application::scrutiny_user_command_callback(
    uint_least8_t const subfunction,
    unsigned char const *request_data,
    uint16_t const request_data_length,
    unsigned char *response_data,
    uint16_t *response_data_length,
    uint16_t const response_max_data_length)
{
	if (subfunction == 1 && request_data_length >=4)
	{
		std::memcpy(&s_requested_led_freq, &request_data[0], sizeof(float));
		s_requested_led_freq = std::fmin(std::fmax(s_requested_led_freq, 0.1f), 10.0f);
	}
}
