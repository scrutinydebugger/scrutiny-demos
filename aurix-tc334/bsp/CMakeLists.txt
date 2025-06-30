#    CMakeLists.txt
#        A manually written CMake file on top of the auto-generated code (iLLD and hardware
#        config) by AURIX Development Studio after creating a blank project for kit_a2g_tc334_lite.
#        Does not ,ake eveything available, only the necessary drivers.
#
#   - License : MIT - See LICENSE file.
#   - Project :  Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
#
#   Copyright (c) 2025 Scrutiny Debugger

cmake_minimum_required(VERSION 3.16.3)



set(LIBRARIES_FOLDER ${CMAKE_CURRENT_LIST_DIR}/Libraries)
set(CONFIG_FOLDER ${CMAKE_CURRENT_LIST_DIR}/Configurations)
set(INFRA_FOLDER ${LIBRARIES_FOLDER}/Infra)
set(ILLD_FOLDER ${LIBRARIES_FOLDER}/iLLD/TC3xx/Tricore)
set(SERVICE_FOLDER ${LIBRARIES_FOLDER}/Service)

FILE(GLOB ImplSources ${ILLD_FOLDER}/_Impl/*.c)
FILE(GLOB PinMapsources ${ILLD_FOLDER}/_PinMap/TC33x/*TQFP144.c)

project(startup_code)


project(bsp)

add_library(${PROJECT_NAME} OBJECT 
    ${CONFIG_FOLDER}/Debug/sync_on_halt.c
    ${CONFIG_FOLDER}/Ifx_Cfg_Ssw.c
    ${CONFIG_FOLDER}/Ifx_Cfg_SswBmhd.c

    ${ILLD_FOLDER}/_Lib/DataHandling/Ifx_CircularBuffer.asm.c
    ${ILLD_FOLDER}/_Lib/DataHandling/Ifx_CircularBuffer.c
    ${ILLD_FOLDER}/_Lib/DataHandling/Ifx_Fifo.c
    ${ILLD_FOLDER}/_Lib/InternalMux/Ifx_InternalMux.c
    
    ${ImplSources}
    ${PinMapsources}

    ${INFRA_FOLDER}/Ssw/TC3xx/Tricore/Ifx_Ssw_Infra.c
    ${INFRA_FOLDER}/Ssw/TC3xx/Tricore/Ifx_Ssw_Tc0.c
    ${INFRA_FOLDER}/Ssw/TC3xx/Tricore/Ifx_Ssw_Tc1.c
    ${INFRA_FOLDER}/Ssw/TC3xx/Tricore/Ifx_Ssw_Tc2.c
    ${INFRA_FOLDER}/Ssw/TC3xx/Tricore/Ifx_Ssw_Tc3.c
    ${INFRA_FOLDER}/Ssw/TC3xx/Tricore/Ifx_Ssw_Tc4.c
    ${INFRA_FOLDER}/Ssw/TC3xx/Tricore/Ifx_Ssw_Tc5.c

    ${INFRA_FOLDER}/Platform/Tricore/Compilers/CompilerDcc.c
    ${INFRA_FOLDER}/Platform/Tricore/Compilers/CompilerGcc.c
    ${INFRA_FOLDER}/Platform/Tricore/Compilers/CompilerGnuc.c
    ${INFRA_FOLDER}/Platform/Tricore/Compilers/CompilerHighTec.c
    ${INFRA_FOLDER}/Platform/Tricore/Compilers/CompilerTasking.c

    ${ILLD_FOLDER}/Asclin/Asc/IfxAsclin_Asc.c
    ${ILLD_FOLDER}/Asclin/Std/IfxAsclin.c
    
    ${ILLD_FOLDER}/Cpu/Std/IfxCpu.c
    ${ILLD_FOLDER}/Cpu/Irq/IfxCpu_Irq.c
    ${ILLD_FOLDER}/Cpu/Trap/IfxCpu_Trap.c

    ${ILLD_FOLDER}/Scu/Std/IfxScuCcu.c
    ${ILLD_FOLDER}/Scu/Std/IfxScuEru.c
    ${ILLD_FOLDER}/Scu/Std/IfxScuLbist.c
    ${ILLD_FOLDER}/Scu/Std/IfxScuRcu.c
    ${ILLD_FOLDER}/Scu/Std/IfxScuWdt.c

    ${ILLD_FOLDER}/Port/Io/IfxPort_Io.c
    ${ILLD_FOLDER}/Port/Std/IfxPort.c
    
    ${ILLD_FOLDER}/Pms/Std/IfxPmsEvr.c
    ${ILLD_FOLDER}/Pms/Std/IfxPmsPm.c
)


target_include_directories(${PROJECT_NAME} PUBLIC
    ${CONFIG_FOLDER}    
    ${INFRA_FOLDER}/Platform
    ${INFRA_FOLDER}/Ssw/TC3xx/Tricore
    ${INFRA_FOLDER}/Sfr/TC33x/
    
    ${SERVICE_FOLDER}/CpuGeneric/
    
    ${ILLD_FOLDER}
    ${ILLD_FOLDER}/Asclin/Std
    ${ILLD_FOLDER}/Asclin/Asc
    ${ILLD_FOLDER}/Cpu/Std
    ${ILLD_FOLDER}/Cpu/Irq/
    ${ILLD_FOLDER}/Scu/Std

    ${ILLD_FOLDER}/_Lib/DataHandling
    ${ILLD_FOLDER}/_Lib/InternalMux

    ${ILLD_FOLDER}/_PinMap/
    ${ILLD_FOLDER}/_PinMap/TC33x
)

# Code comes from Infineon. Suppress warnings that they cause
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unused-parameter) 