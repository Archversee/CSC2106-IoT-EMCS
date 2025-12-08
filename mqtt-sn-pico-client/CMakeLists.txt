# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global include paths for all targets
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/fs)
include_directories(${CMAKE_CURRENT_LIST_DIR}/drivers)
include_directories(${CMAKE_CURRENT_LIST_DIR}/mqtt)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../paho.mqtt-sn.embedded-c-master/MQTTSNPacket/src)
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

# set(PICO_NO_PICOTOOL 1)


include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)




# Prevent CMake from adding macOS-specific flags to cross-compiler
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(mqtt-sn-pico-client C CXX ASM)

# Fetch FreeRTOS Kernel
include(FetchContent)

# Set FreeRTOS config file directory
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" CACHE STRING "")

# Set FreeRTOS port for RP2040 (uses ThirdParty port)
set(FREERTOS_PORT GCC_RP2040 CACHE STRING "")

FetchContent_Declare(
    FreeRTOS-Kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG        V10.5.1
)
FetchContent_MakeAvailable(FreeRTOS-Kernel)

# Get the FreeRTOS source directory
FetchContent_GetProperties(FreeRTOS-Kernel)
if(NOT freertos-kernel_POPULATED)
    FetchContent_Populate(FreeRTOS-Kernel)
endif()

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add FatFS library subdirectory
add_subdirectory(no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)

# Add subdirectories for drivers and fs
add_subdirectory(drivers)
add_subdirectory(fs)

# Create mqtt-client library for shared functionality
add_library(mqtt_client STATIC
    mqtt-client.c
)

target_link_libraries(mqtt_client
    microsd_driver
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    freertos_kernel
    freertos_kernel_port
    fs_lib
)

target_include_directories(mqtt_client PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${freertos-kernel_SOURCE_DIR}/include
    ${freertos-kernel_SOURCE_DIR}/portable/ThirdParty/GCC/RP2040/include
)


add_executable(pico-client-pub
    pico-client-pub.c
    mqtt/mqtt-sn-udp.c
    hw_config.c
)

add_executable(pico-client-sub
    pico-client-sub.c
    mqtt/mqtt-sn-udp.c
    hw_config.c
)

# pico_set_program_name(mqtt-sn-pico-client "mqtt-sn-pico-client")
# pico_set_program_version(mqtt-sn-pico-client "0.1")

pico_set_program_name(pico-client-pub "pico-client-pub")
pico_set_program_version(pico-client-pub "0.1")

pico_set_program_name(pico-client-sub "pico-client-sub")
pico_set_program_version(pico-client-sub "0.1")

# Add the standard include files to the build
# target_include_directories(mqtt-sn-pico-client PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
#         ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
#         ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
#         ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
# )

target_include_directories(pico-client-pub PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
)

target_include_directories(pico-client-sub PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
        ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
)

# Define PICO_TX_BUILD for the TX client to use correct client ID
target_compile_definitions(pico-client-pub PRIVATE PICO_TX_BUILD)

# include in driver and fs directories

# target_link_libraries(mqtt-sn-pico-client
#     pico_stdlib
#     hardware_spi
#     hardware_gpio
#     no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
#     fs_lib
#     pico_cyw43_arch_lwip_poll
#     hardware_timer
target_link_libraries(pico-client-sub
    pico_stdlib
    hardware_spi
    hardware_gpio
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    microsd_driver
    mqtt_client
    fs_lib
    pico_cyw43_arch_lwip_threadsafe_background
    freertos_kernel
    freertos_kernel_port
    hardware_timer
)
target_link_libraries(pico-client-pub
    pico_stdlib
    hardware_spi
    hardware_gpio
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    microsd_driver
    mqtt_client
    fs_lib
    pico_cyw43_arch_lwip_threadsafe_background
    freertos_kernel
    freertos_kernel_port
    hardware_timer
)

# Enable USB output, disable UART output
# pico_enable_stdio_usb(mqtt-sn-pico-client 1)
# pico_enable_stdio_uart(mqtt-sn-pico-client 0)

pico_enable_stdio_usb(pico-client-sub 1)
pico_enable_stdio_uart(pico-client-sub 0)
pico_enable_stdio_usb(pico-client-pub 1)
pico_enable_stdio_uart(pico-client-pub 0)

# Create map/bin/hex/uf2 files
# pico_add_extra_outputs(mqtt-sn-pico-client)
pico_add_extra_outputs(pico-client-sub)
pico_add_extra_outputs(pico-client-pub)