# MicroSD Driver Library
add_library(microsd_driver
    microsd_driver.c
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

# Large File Test Executable
add_executable(large_file_test
    tests/large_file_test.c
)

target_link_libraries(large_file_test
    microsd_driver
    pico_stdlib
)

# Enable USB output and disable UART output
pico_enable_stdio_usb(large_file_test 1)
pico_enable_stdio_uart(large_file_test 0)

# Create UF2 file
pico_add_extra_outputs(large_file_test)
