
cmake_minimum_required(VERSION 3.16)

project(scrutiny-nsec2024)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION 0)  # Prevent adding -flto, incompatible with -g 

include(cmake/arduino_core.cmake)

include(FetchContent)

set(SCRUTINY_ENABLE_DATALOGGING 1)
set(SCRUTINY_SUPPORT_64BITS 0) 
set(SCRUTINY_DATALOGGING_BUFFER_32BITS 0) 

FetchContent_Declare(
  scrutiny-embedded
  GIT_REPOSITORY https://github.com/scrutinydebugger/scrutiny-embedded.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(scrutiny-embedded)

add_compile_options(
    -Os
    -Wall 
    -Wextra 
    -Werror 
    -gdwarf-4   # Necessary for Scrutiny
)

target_link_libraries(scrutiny-embedded PUBLIC ArduinoCore) # Will inherit the build flags

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/scrutiny_integration.cpp
    src/bno055.cpp
    src/twi/twi.c
)
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")


target_include_directories(${PROJECT_NAME} PUBLIC
    src
    src/twi
)


target_link_libraries(${PROJECT_NAME} PRIVATE 
    ArduinoCore
    scrutiny-embedded
)

# Scrutiny postbuild tools. Create a new .elf with the firmware ID injected + Create the .sfd file
scrutiny_postbuild(${PROJECT_NAME} 
    TAGGED_EXECUTABLE_TARGET_VAR TAGGED_BIN_TARGET
    METADATA_PROJECT_NAME "Atmega 2560 Demo"
    METADATA_VERSION "1.0.0"
    METADATA_AUTHOR "Scrutiny Debugger"
    ALIAS_FILES aliases/alias1.json
    CPPFILT ${CMAKE_CPPFILT}
    INSTALL_SFD
)

#Generates the .eepr file
add_custom_command(TARGET ${TAGGED_BIN_TARGET} POST_BUILD
    BYPRODUCTS ${TAGGED_BIN_TARGET}.eep
    COMMAND ${CMAKE_OBJCOPY} ARGS 
        -O ihex -j .eeprom
        --set-section-flags=.eeprom=alloc,load
        --no-change-warnings --change-section-lma 
        .eeprom=0
        ${TAGGED_BIN_TARGET}.elf
        ${TAGGED_BIN_TARGET}.eep
)

# Generate the .hex
add_custom_command(TARGET ${TAGGED_BIN_TARGET}  POST_BUILD
    DEPENDS ${TAGGED_BIN_TARGET}.elf
    BYPRODUCTS ${TAGGED_BIN_TARGET}.hex
    COMMAND ${CMAKE_OBJCOPY} ARGS 
        -O ihex -R .eeprom
        ${TAGGED_BIN_TARGET}.elf
        ${TAGGED_BIN_TARGET}.hex
)

# Prints the size to the console
add_custom_command(TARGET ${TAGGED_BIN_TARGET}  POST_BUILD
    DEPENDS ${TAGGED_BIN_TARGET}.elf
    COMMAND ${CMAKE_SIZE} ARGS 
        -A --mcu=${ARDUINO_MCU}
        ${TAGGED_BIN_TARGET}.elf
)

