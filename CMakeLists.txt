cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
if(NOT DEFINED PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    else()
        message(FATAL_ERROR "PICO_SDK_PATH is not set. Please set it to the pico-sdk directory.")
    endif()
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(inf2004_project C CXX ASM)

include_directories(${CMAKE_CURRENT_LIST_DIR}/mqtt-sn-pico-client)

pico_sdk_init()

# # Add drivers subdirectory
# add_subdirectory(drivers)

# # MicroSD Demo executable
# add_executable(microsd_demo
#     microsd_demo.c
# )

# target_link_libraries(microsd_demo
#     pico_stdlib
#     hardware_spi
#     hardware_gpio
#     microsd_driver
# )

# MicroSD
#pico_enable_stdio_usb(microsd_demo 1)
#pico_enable_stdio_uart(microsd_demo 0)

#pico_add_extra_outputs(microsd_demo)

# MQTT-SN

# MQTT-SN Pico Client executable
add_executable(mqtt-sn-pico-client
    mqtt-sn-pico-client/mqtt-sn-pico-client.c
    mqtt-sn-pico-client/functions.c
)

target_link_libraries(mqtt-sn-pico-client
    pico_stdlib
    pico_cyw43_arch_lwip_poll
    hardware_timer
)

pico_enable_stdio_usb(mqtt-sn-pico-client 1)
pico_enable_stdio_uart(mqtt-sn-pico-client 0)

pico_add_extra_outputs(mqtt-sn-pico-client)

# Data frame executable (existing)
# add_executable(data_frame_demo
#     large-file-reconstruction/data_frame.c
# )

# target_link_libraries(data_frame_demo
#     pico_stdlib
# )

# pico_add_extra_outputs(data_frame_demo)
