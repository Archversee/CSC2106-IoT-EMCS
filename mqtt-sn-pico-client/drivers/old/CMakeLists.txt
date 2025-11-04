# MicroSD Driver Library (now modularized)
add_library(microsd_driver
    microsd_driver.c
    microsd_exfat.c
    microsd_file.c
    microsd_chunk.c
    microsd_util.c
    microsd_spi.c
)

target_include_directories(microsd_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(microsd_driver
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# Test executables
# MicroSD Demo Executable
add_executable(microsd_demo
    tests/microsd_demo.c
)

target_link_libraries(microsd_demo
    microsd_driver
    pico_stdlib
)

# Enable USB output and disable UART output
pico_enable_stdio_usb(microsd_demo 1)
pico_enable_stdio_uart(microsd_demo 0)

# Create UF2 file
pico_add_extra_outputs(microsd_demo)

# Automated MicroSD Test Executable
add_executable(automated_microsd_test
    tests/automated_microsd_test.c
)

target_link_libraries(automated_microsd_test
    microsd_driver
    pico_stdlib
)

# Enable USB output and disable UART output
pico_enable_stdio_usb(automated_microsd_test 1)
pico_enable_stdio_uart(automated_microsd_test 0)

# Create UF2 file
pico_add_extra_outputs(automated_microsd_test)
