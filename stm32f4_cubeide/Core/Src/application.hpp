
#ifndef SRC_APPLICATION_HPP_
#define SRC_APPLICATION_HPP_

#include "scrutiny_rtt_integration.hpp"
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


	ScrutinyRTTIntegration m_scrutiny_integration;
};


#endif /* SRC_APPLICATION_HPP_ */
