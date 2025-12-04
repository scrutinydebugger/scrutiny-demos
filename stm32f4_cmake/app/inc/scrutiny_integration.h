#ifndef __SCRUTINY_INTEGRATION_H
#define __SCRUTINY_INTEGRATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void scrutiny_integration_init();
void scrutiny_integration_update(const uint32_t timestamp_us);

#ifdef __cplusplus
}
#endif

#endif