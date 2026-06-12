#include "scrutiny.hpp"
#include <stdint.h>

extern scrutiny::FixedFrequencyLoopHandler task1KHz_LoopHandler;
extern bool scrutiny_init(uint32_t const sci_base);
extern void scrutiny_idle_update(uint32_t const timediff_100ns);