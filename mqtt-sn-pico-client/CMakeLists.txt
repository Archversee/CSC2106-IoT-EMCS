# Generated Cmake Pico project file

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

project(mqtt_sn_pico_client C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add drivers subdirectory
add_subdirectory(drivers)
add_subdirectory(fs)

# MQTT-SN Pico Client executable
add_executable(mqtt_sn_pico_client
    main.c
)

target_link_libraries(mqtt_sn_pico_client
    pico_stdlib
    hardware_spi
    hardware_gpio
    microsd_driver
    fs_lib
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(mqtt_sn_pico_client 1)
pico_enable_stdio_uart(mqtt_sn_pico_client 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(mqtt_sn_pico_client)