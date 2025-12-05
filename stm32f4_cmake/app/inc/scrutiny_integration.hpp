//    scrutiny_integration.h
//        The app wrapper that integrates scrutiny in this demo by connecting the USB virtual
//        serial port (CDC-ACM) to the scrutiny streams.
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//   - Author : MrMati (Mateusz Niedbała) <mati.niedbala5@gmail.com>
//   - Contributors :
//       - Pier-Yves Lessard (pylessard)
//
//    Copyright (c) 2025 Scrutiny Debugger

#ifndef __SCRUTINY_INTEGRATION_H
#define __SCRUTINY_INTEGRATION_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    void scrutiny_integration_init();
    void scrutiny_integration_update(const uint32_t timestamp_us);

#ifdef __cplusplus
}
#endif

#endif