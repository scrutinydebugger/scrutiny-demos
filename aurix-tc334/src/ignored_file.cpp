//    ignored_file.cpp
//        A file that demonstrate the ability to filter out compile units from the postbuild
//        toolchain
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

/*
None of these variables will appear in Scrutiny as they will be filtered out of the SFD during the postbuild stage.
In fact, whole compile unit shouldn't be there (/static/ignored_file.cpp)
The cmake requires to ignore that file. See: CU_IGNORE_PATTERNS
*/
static volatile int ignored_static_int = 0;
volatile int ignored_global_int = 0;
