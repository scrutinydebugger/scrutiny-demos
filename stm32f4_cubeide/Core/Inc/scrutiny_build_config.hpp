//    scrutiny_build_config.hpp
//        A header to be maintained for Scrutiny. Required for non-cmake integrations. Would
//        be auto-generated with CMake.
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//    Copyright (c) 2026 Scrutiny Debugger

#ifndef ___SCRUTINY_DEFAULT_BUILD_CONFIG_H___
#define ___SCRUTINY_DEFAULT_BUILD_CONFIG_H___

#ifndef SCRUTINY_SUPPORT_64BITS
#define SCRUTINY_SUPPORT_64BITS 0
#endif

#ifndef SCRUTINY_ENABLE_DATALOGGING
#define SCRUTINY_ENABLE_DATALOGGING 1
#endif

#ifndef SCRUTINY_SUPPORT_PROTECTED_REGIONS
#define SCRUTINY_SUPPORT_PROTECTED_REGIONS 0
#endif

#define SCRUTINY_REQUEST_MAX_PROCESS_TIME_US 100000u
#define SCRUTINY_COMM_RX_TIMEOUT_US 50000u
#define SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US 5000000u
#define SCRUTINY_ACTUAL_PROTOCOL_VERSION SCRUTINY_PROTOCOL_VERSION(1, 0u)

#if SCRUTINY_ENABLE_DATALOGGING
#define SCRUTINY_DATALOGGING_MAX_SIGNAL 16u
#define SCRUTINY_DATALOGGING_ENCODING SCRUTINY_DATALOGGING_ENCODING_RAW
#define SCRUTINY_DATALOGGING_BUFFER_32BITS 0
#endif

#endif //___SCRUTINY_DEFAULT_BUILD_CONFIG_H___
