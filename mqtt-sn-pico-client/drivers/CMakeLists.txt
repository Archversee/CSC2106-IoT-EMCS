# MicroSD Driver Library (High-level FatFS wrapper)
add_library(microsd_driver
    microsd_driver.c
)

target_include_directories(microsd_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
    ${CMAKE_CURRENT_SOURCE_DIR}/../no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
)

target_link_libraries(microsd_driver
    pico_stdlib
    hardware_spi
    hardware_gpio
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)
