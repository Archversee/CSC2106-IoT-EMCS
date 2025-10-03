cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(inf2004_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Add drivers subdirectory
add_subdirectory(drivers)

# MicroSD Demo executable
add_executable(microsd_demo
    microsd_demo.c
)

target_link_libraries(microsd_demo
    pico_stdlib
    hardware_spi
    hardware_gpio
    microsd_driver
)

pico_enable_stdio_usb(microsd_demo 1)
pico_enable_stdio_uart(microsd_demo 0)

pico_add_extra_outputs(microsd_demo)

# Data frame executable (existing)
add_executable(data_frame_demo
    large-file-reconstruction/data_frame.c
)

target_link_libraries(data_frame_demo
    pico_stdlib
)

pico_add_extra_outputs(data_frame_demo)
