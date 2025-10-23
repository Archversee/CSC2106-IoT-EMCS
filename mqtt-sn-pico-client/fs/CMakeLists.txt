# File System Library
add_library(fs_lib
    data_frame.c
    chunk_transfer.c
)

target_include_directories(fs_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fs_lib
    pico_stdlib
    microsd_driver
)

# Data Frame Demo Executable
add_executable(data_frame_demo
    data_frame_demo.c
)

target_link_libraries(data_frame_demo
    pico_stdlib
    fs_lib
    microsd_driver
)

# Enable USB output for demo
pico_enable_stdio_usb(data_frame_demo 1)
pico_enable_stdio_uart(data_frame_demo 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(data_frame_demo)

# Chunk Write Example Executable
add_executable(chunk_write_example
    chunk_write_example.c
)

target_link_libraries(chunk_write_example
    pico_stdlib
    fs_lib
    microsd_driver
)

# Enable USB output for example
pico_enable_stdio_usb(chunk_write_example 1)
pico_enable_stdio_uart(chunk_write_example 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(chunk_write_example)
