cmake_minimum_required(VERSION 3.21)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME stm32_demo)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)


project(${CMAKE_PROJECT_NAME})
enable_language(C CXX ASM)

add_compile_options(
    -gdwarf-4   # Necessary for Scrutiny
)

add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)


# Set .elf suffix for tagged binary
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

set(SCRUTINY_ENABLE_DATALOGGING 1)
set(SCRUTINY_SUPPORT_64BITS 0) 
set(SCRUTINY_BUILD_CWRAPPER 0)

include(FetchContent)
FetchContent_Declare(
  scrutiny-embedded
  GIT_REPOSITORY https://github.com/scrutinydebugger/scrutiny-embedded.git
  GIT_TAG        v0.3.1
)
FetchContent_MakeAvailable(scrutiny-embedded)


target_link_libraries(${PROJECT_NAME} stm32cubemx scrutiny-embedded)

scrutiny_postbuild(${PROJECT_NAME} 
    TAGGED_EXECUTABLE_TARGET_VAR TAGGED_BIN_TARGET
    TAGGED_EXECUTABLE_PATH_VAR TAGGED_BIN_FILE
    METADATA_PROJECT_NAME "STM32 Demo"
    METADATA_VERSION "1.1.0"
    METADATA_AUTHOR "Scrutiny Debugger"
    ALIAS_FILES aliases.json
    CPPFILT ${CMAKE_CPPFILT}
    CU_IGNORE_PATTERNS
        ${CMAKE_SOURCE_DIR}"/Drivers/*"
        ${CMAKE_SOURCE_DIR}"/Middlewares/*"
        ${CMAKE_SOURCE_DIR}"/USB_DEVICE/*"
    INSTALL_SFD
)

get_filename_component(TAGGED_BINARY_BASENAME_WE ${TAGGED_BIN_FILE} NAME_WE)

# Generate .hex file from the tagged ELF
add_custom_command(TARGET ${TAGGED_BIN_TARGET} POST_BUILD
     BYPRODUCTS ${TAGGED_BINARY_BASENAME_WE}.hex
     VERBATIM 
     COMMAND ${CMAKE_OBJCOPY} ARGS 
        -O ihex
        "${TAGGED_BIN_FILE}"
        "${TAGGED_BINARY_BASENAME_WE}.hex"
    COMMENT "Generating ${TAGGED_BINARY_BASENAME_WE}.hex from ELF"
)