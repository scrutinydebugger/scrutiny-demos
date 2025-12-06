#    CMakeLists.txt
#        The application main Cmake entry point
#
#   - License : MIT - See LICENSE file
#   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
#   - Author : MrMati (Mateusz Niedbała) <mati.niedbala5@gmail.com>
#   - Contributors : 
#       - Pier-Yves Lessard (pylessard)
#
#    Copyright (c) 2025 Scrutiny Debugger

cmake_minimum_required(VERSION 3.16.3)

set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/toolchain/STM32F411XX_FLASH.ld)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_language(C CXX ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

project(stm32f4_demo)

add_compile_definitions(
	USE_HAL_DRIVER 
	STM32F411xE
    $<$<CONFIG:Debug>:DEBUG>  # add DEBUG define only for Debug build
)

set(SCRUTINY_ENABLE_DATALOGGING 1)
set(SCRUTINY_SUPPORT_64BITS 0) 
set(SCRUTINY_BUILD_CWRAPPER 1)

include(FetchContent)
FetchContent_Declare(
  scrutiny-embedded
  GIT_REPOSITORY https://github.com/scrutinydebugger/scrutiny-embedded.git
  GIT_TAG        v0.4.0
)

FetchContent_MakeAvailable(scrutiny-embedded)

include_directories(global_inc)    # Global include required for stm32cube generated files
add_compile_options(
    -gdwarf-4   # Necessary for Scrutiny
)

add_subdirectory(stm32cubemx)   # Load stm32cubemx generated files

add_executable(${PROJECT_NAME}
    src/scrutiny_integration.c
    src/main.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")
target_link_options(${PROJECT_NAME} PRIVATE -T${LINKER_SCRIPT})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,-Map=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.map)

target_include_directories(${PROJECT_NAME} PUBLIC
    inc
)
target_link_libraries(${PROJECT_NAME} 
    stm32cubemx         # Object lib in this proejct
    scrutiny-cwrapper   # Static lib coming from Github
)

# Inject the firmware ID in the .elf and generate the .sfd file for the server
scrutiny_postbuild(${PROJECT_NAME} 
    TAGGED_EXECUTABLE_TARGET_VAR TAGGED_BIN_TARGET
    TAGGED_EXECUTABLE_PATH_VAR TAGGED_BIN_FILE
    METADATA_PROJECT_NAME "STM32F4 Demo"
    METADATA_VERSION "1.0.0"
    METADATA_AUTHOR "MrMati (Mateusz Niedbała)"
    ALIAS_FILES aliases.json
    CPPFILT ${CMAKE_CPPFILT}
    INSTALL_SFD     # Auto install the .sfd on the build amchine
)

get_filename_component(TAGGED_BINARY_BASENAME_WE ${TAGGED_BIN_FILE} NAME_WE)

# Generate .hex file from the tagged ELF
set(HEX_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TAGGED_BINARY_BASENAME_WE}.hex")
add_custom_command(
    TARGET ${TAGGED_BIN_TARGET} POST_BUILD
    DEPENDS ${TAGGED_BIN_FILE}
    BYPRODUCTS ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY}
        -O ihex
        "${TAGGED_BIN_FILE}"
        "${HEX_FILE}"
    COMMENT "Generating ${HEX_FILE}"
    VERBATIM
)

