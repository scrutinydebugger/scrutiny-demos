/*
 * ScrutinyIntegration.cpp
 *
 *  Created on: Aug 16, 2026
 *      Author: Pier-Yves
 */

#include "scrutiny_rtt_integration.hpp"
#include "scrutiny.hpp"
#include "SEGGER_RTT.h"

ScrutinyRTTIntegration::ScrutinyRTTIntegration(unsigned int const rtt_buffer) :
	m_rtt_buffer(rtt_buffer),
	m_main_handler(),
	m_1khz_loop_handler(1e7/1000, "1KHz"),	// (1e7 / freq_hz) to get the number of 100ns steps per cycle
	m_idle_loop_handler("Idle")
{

}

void ScrutinyRTTIntegration::init(void)
{
	scrutiny::Config config;
	config.set_buffers(m_rx_buffer, sizeof(m_rx_buffer), m_tx_buffer, sizeof(m_tx_buffer));
	config.set_loops(m_loop_handlers, sizeof(m_loop_handlers)/sizeof(m_loop_handlers[0]));
	config.display_name = "STM32F4 RTT demo";

	m_main_handler.init(&config);
}


void ScrutinyRTTIntegration::update(uint32_t const timediff_100ns)
{
	unsigned char buffer[16];
	unsigned int const nb_read = SEGGER_RTT_Read(m_rtt_buffer, buffer, sizeof(buffer));
	if (nb_read > 0)
	{
		m_main_handler.receive_data(buffer, nb_read);
	}
	m_main_handler.process(timediff_100ns);
	uint16_t const nb_to_send = m_main_handler.pop_data(buffer, sizeof(buffer));
	if (nb_to_send > 0)
	{
		SEGGER_RTT_Write(m_rtt_buffer, buffer, nb_to_send);
	}
}

void ScrutinyRTTIntegration::loop_1khz_exec(uint32_t const timediff_100ns)
{
	m_1khz_loop_handler.process(timediff_100ns);
}

