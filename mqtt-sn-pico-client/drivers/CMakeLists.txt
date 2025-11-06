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

# Multi-File Write Test Executable
add_executable(multi_file_write_test
    tests/multi_file_write_test.c
    ../hw_config.c
)

target_link_libraries(multi_file_write_test
    pico_stdlib
    hardware_spi
    microsd_driver
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# Enable USB output for test
pico_enable_stdio_usb(multi_file_write_test 1)
pico_enable_stdio_uart(multi_file_write_test 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(multi_file_write_test)
