#    tricore-gcc.cmake
#        A toolchain file to enable the Aurix GCC toolchain with cmake
#
#   - License : MIT - See LICENSE file.
#   - Project :  Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
#
#   Copyright (c) 2025 Scrutiny Debugger

cmake_minimum_required(VERSION 3.16.3)

include(CMakeForceCompiler)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SIZEOF_VOID_P 4)

set(TOOLCHAIN tricore-elf)

set(CMAKE_C_COMPILER   ${TOOLCHAIN}-gcc${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}-g++${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "ASM Compiler")
set(CMAKE_AR   ${TOOLCHAIN}-gcc-ar${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "Ar binary")
set(CMAKE_RANLIB   ${TOOLCHAIN}-gcc-ranlib${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "Ranlib binary")
set(CMAKE_LINKER   ${TOOLCHAIN}-ld${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "Linker binary")
set(CMAKE_OBJCOPY  ${TOOLCHAIN}-objcopy${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "Objcopy binary")
set(CMAKE_CPPFILT  ${TOOLCHAIN}-c++filt${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "c++filt binary")
set(CMAKE_SIZE ${TOOLCHAIN}-size${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "Size binary")
set(CMAKE_STRIP${TOOLCHAIN}-strip${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "Strip binary")

set(CMAKE_C_COMPILER_WORKS   1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

set(CMAKE_C_FLAGS "-Wall -Wextra -fmessage-length=0 -ffunction-sections -fdata-sections -mtc162 -MMD -MP" CACHE INTERNAL "C Flags")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE INTERNAL "C++ Flags")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "ASM Flags")
set(CMAKE_EXE_LINKER_FLAGS "-mtc162 -nocrt0 -Wl,--gc-sections" CACHE INTERNAL "Linker options")

set(CMAKE_C_FLAGS_DEBUG  "-g -Og -DDEBUG" CACHE INTERNAL "C Debug Flags")
set(CMAKE_CXX_FLAGS_DEBUG"-g -Og -DDEBUG" CACHE INTERNAL "C++ Debug Flags")
set(CMAKE_ASM_FLAGS_DEBUG"-g -DDEBUG" CACHE INTERNAL "ASM Debug Flags")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG ""   CACHE INTERNAL "Linker Debug Flags")

set(CMAKE_C_FLAGS_RELEASE  "-O3 -DNDEBUG" CACHE INTERNAL "C Release Flags")
set(CMAKE_CXX_FLAGS_RELEASE"-O3 -DNDEBUG" CACHE INTERNAL "C++ Release Flags")
set(CMAKE_ASM_FLAGS_RELEASE"-DNDEBUG" CACHE INTERNAL "ASM Release Flags")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "" CACHE INTERNAL "Linker Release Flags")