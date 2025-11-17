# File System Library
add_library(fs_lib
    data_frame.c
    chunk_transfer.c
)

include_directories(fs_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
    ${CMAKE_CURRENT_SOURCE_DIR}/../no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
)

target_link_libraries(fs_lib
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_dma
    hardware_pio
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    microsd_driver
)

# Serialize/Deserialize Test Executable
add_executable(serialize_test
    tests/serialize_test.c
)

target_link_libraries(serialize_test
    pico_stdlib
    fs_lib
)

# Enable USB output for test
pico_enable_stdio_usb(serialize_test 1)
pico_enable_stdio_uart(serialize_test 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(serialize_test)

add_executable(streaming_test
    tests/streaming_test.c
    ../hw_config.c
)

target_link_libraries(streaming_test
    pico_stdlib
    hardware_spi
    fs_lib
    microsd_driver
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# Enable USB output for test
pico_enable_stdio_usb(streaming_test 1)
pico_enable_stdio_uart(streaming_test 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(streaming_test)

# Automated Chunk Transfer Test Executable
add_executable(automated_chunk_test
    tests/automated_chunk_test.c
    ../hw_config.c
)

target_link_libraries(automated_chunk_test
    pico_stdlib
    fs_lib
    microsd_driver
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# Enable USB output for test
pico_enable_stdio_usb(automated_chunk_test 1)
pico_enable_stdio_uart(automated_chunk_test 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(automated_chunk_test)

# Automated Streaming Test Executable
add_executable(automated_streaming_test
    tests/automated_streaming_test.c
    ../hw_config.c
)

target_link_libraries(automated_streaming_test
    pico_stdlib
    fs_lib
    microsd_driver
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# Enable USB output for test
pico_enable_stdio_usb(automated_streaming_test 1)
pico_enable_stdio_uart(automated_streaming_test 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(automated_streaming_test)
