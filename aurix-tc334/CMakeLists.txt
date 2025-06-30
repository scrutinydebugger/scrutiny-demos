cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/toolchain/tricore-gcc.cmake)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/toolchain/Lcf_Gnuc_Tricore_Tc.lsl)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(scrutiny-aurix-tc334)

add_subdirectory(bsp)

add_executable(${PROJECT_NAME} 
    src/main.c
    src/UART_VCOM.c
    )
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE bsp)

target_link_options(${PROJECT_NAME} PRIVATE -T${LINKER_SCRIPT})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,-Map=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.map)

add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
     DEPENDS ${PROJECT_NAME}.elf
     BYPRODUCTS ${PROJECT_NAME}.hex
     VERBATIM 
     COMMAND ${CMAKE_OBJCOPY} ARGS 
        -O ihex
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.hex"
     )

add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
     DEPENDS ${PROJECT_NAME}.elf
     BYPRODUCTS ${PROJECT_NAME}.lst
     VERBATIM 
     COMMAND ${CMAKE_OBJDUMP} ARGS
        -h -S 
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf"
        > "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.lst"
     )