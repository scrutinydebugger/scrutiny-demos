#ifndef SRC_SCRUTINY_INTEGRATION_HPP_
#define SRC_SCRUTINY_INTEGRATION_HPP_

#include "scrutiny.hpp"
#include <cstdint>

class ScrutinyRTTIntegration {
public:
	ScrutinyRTTIntegration(unsigned int const rtt_buffer);
	void init(scrutiny::user_command_callback_t user_command);
	void update(uint32_t const timediff_100ns);
	void loop_1khz_exec(uint32_t const timediff_100ns);

private:

	unsigned int const m_rtt_buffer;
	scrutiny::MainHandler m_main_handler;
	scrutiny::FixedFrequencyLoopHandler m_1khz_loop_handler;
	scrutiny::VariableFrequencyLoopHandler m_idle_loop_handler;
	scrutiny::LoopHandler* m_loop_handlers[2] = {&m_1khz_loop_handler, &m_idle_loop_handler};

	unsigned char m_tx_buffer[256];
	unsigned char m_rx_buffer[128];
	unsigned char m_datalogging_buffer[4096];
};

#endif /* SRC_SCRUTINY_INTEGRATION_HPP_ */
