//    application.hpp
//        The high level application invoked from main
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//    Copyright (c) 2026 Scrutiny Debugger

#ifndef SRC_APPLICATION_HPP_
#define SRC_APPLICATION_HPP_

#include "scrutiny_rtt_integration.hpp"
#include "wave_function_generator.hpp"
#include "stm32f4xx_hal.h"
#include <cstdint>

class Application
{
public:
	Application(void);
	void init(void);
	int run(void);
	void task_1khz(void);
	inline uint32_t tick_1us(void)
	{
		return TIM2->CNT;
	}


private:
	static void scrutiny_user_command_callback(
	    uint_least8_t const subfunction,
	    unsigned char const *request_data,
	    uint16_t const request_data_length,
	    unsigned char *response_data,
	    uint16_t *response_data_length,
	    uint16_t const response_max_data_length);

	static float s_requested_led_freq;
	ScrutinyRTTIntegration m_scrutiny_integration;

	WaveFunctionGenerator m_main_loop_generator;
	WaveFunctionGenerator m_isr_1khz_generator;
	bool volatile m_user_key_pressed;
	bool volatile m_sync_all_wavegen;
};


#endif /* SRC_APPLICATION_HPP_ */
