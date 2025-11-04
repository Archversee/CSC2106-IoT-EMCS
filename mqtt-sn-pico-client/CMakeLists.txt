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

# Prevent CMake from adding macOS-specific flags to cross-compiler
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(mqtt-sn-pico-client C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add FatFS library subdirectory
add_subdirectory(no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)

# Add subdirectories for drivers and fs
add_subdirectory(drivers)
add_subdirectory(fs)

# MQTT-SN Pico Client executable (commented out - using split TX/RX versions)
# add_executable(mqtt-sn-pico-client
#     #main.c
#     mqtt-sn-pico-client.c
#     mqtt/mqtt-sn-udp.c
#     hw_config.c
# )

add_executable(mqtt-sn-pico-client-rx
    #main.c
    mqtt-sn-pico-client-rx.c
    mqtt/mqtt-sn-udp.c
    hw_config.c
)

add_executable(mqtt-sn-pico-client-tx
    #main.c
    mqtt-sn-pico-client-tx.c
    mqtt/mqtt-sn-udp.c
    hw_config.c
)

# pico_set_program_name(mqtt-sn-pico-client "mqtt-sn-pico-client")
# pico_set_program_version(mqtt-sn-pico-client "0.1")

pico_set_program_name(mqtt-sn-pico-client-rx "mqtt-sn-pico-client-rx")
pico_set_program_version(mqtt-sn-pico-client-rx "0.1")

pico_set_program_name(mqtt-sn-pico-client-tx "mqtt-sn-pico-client-tx")
pico_set_program_version(mqtt-sn-pico-client-tx "0.1")

# Add the standard include files to the build
# target_include_directories(mqtt-sn-pico-client PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
#         ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
#         ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
#         ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
# )

target_include_directories(mqtt-sn-pico-client-rx PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
)

target_include_directories(mqtt-sn-pico-client-tx PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
)

# Define BUILD_TX_CLIENT for the TX client to use correct client ID
target_compile_definitions(mqtt-sn-pico-client-tx PRIVATE BUILD_TX_CLIENT)

# include in driver and fs directories

# target_link_libraries(mqtt-sn-pico-client
#     pico_stdlib
#     hardware_spi
#     hardware_gpio
#     no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
#     fs_lib
#     pico_cyw43_arch_lwip_poll
#     hardware_timer
# )

target_link_libraries(mqtt-sn-pico-client-rx
    pico_stdlib
    hardware_spi
    hardware_gpio
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    microsd_driver
    fs_lib
    pico_cyw43_arch_lwip_poll
    hardware_timer
)

target_link_libraries(mqtt-sn-pico-client-tx
    pico_stdlib
    hardware_spi
    hardware_gpio
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    microsd_driver
    fs_lib
    pico_cyw43_arch_lwip_poll
    hardware_timer
)

# Enable USB output, disable UART output
# pico_enable_stdio_usb(mqtt-sn-pico-client 1)
# pico_enable_stdio_uart(mqtt-sn-pico-client 0)

pico_enable_stdio_usb(mqtt-sn-pico-client-rx 1)
pico_enable_stdio_uart(mqtt-sn-pico-client-rx 0)

pico_enable_stdio_usb(mqtt-sn-pico-client-tx 1)
pico_enable_stdio_uart(mqtt-sn-pico-client-tx 0)

# Create map/bin/hex/uf2 files
# pico_add_extra_outputs(mqtt-sn-pico-client)
pico_add_extra_outputs(mqtt-sn-pico-client-rx)
pico_add_extra_outputs(mqtt-sn-pico-client-tx)
