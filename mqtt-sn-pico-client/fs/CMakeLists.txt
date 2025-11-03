# File System Library
add_library(fs_lib
    data_frame.c
    chunk_transfer.c
)

include_directories(fs_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PICO_SDK_PATH}/src/common/pico_rand/include  

)

target_link_libraries(fs_lib
    pico_stdlib
    microsd_driver
)

# Test executables
# Data Frame Demo Executable
add_executable(data_frame_demo
    tests/data_frame_demo.c
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
    tests/chunk_write_example.c
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

# File Transfer Complete Demo Executable
add_executable(file_transfer_demo
    tests/file_transfer_demo.c
)

target_link_libraries(file_transfer_demo
    pico_stdlib
    fs_lib
    microsd_driver
)

# Enable USB output for demo
pico_enable_stdio_usb(file_transfer_demo 1)
pico_enable_stdio_uart(file_transfer_demo 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(file_transfer_demo)

# Streaming Test Executable (memory efficient chunk reading)
add_executable(streaming_test
    tests/streaming_test.c
)

target_link_libraries(streaming_test
    pico_stdlib
    fs_lib
    microsd_driver
)

# Enable USB output for test
pico_enable_stdio_usb(streaming_test 1)
pico_enable_stdio_uart(streaming_test 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(streaming_test)

# Automated Chunk Transfer Test Executable
add_executable(automated_chunk_test
    tests/automated_chunk_test.c
)

target_link_libraries(automated_chunk_test
    pico_stdlib
    fs_lib
    microsd_driver
)

# Enable USB output for test
pico_enable_stdio_usb(automated_chunk_test 1)
pico_enable_stdio_uart(automated_chunk_test 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(automated_chunk_test)

# Automated Streaming Test Executable
add_executable(automated_streaming_test
    tests/automated_streaming_test.c
)

target_link_libraries(automated_streaming_test
    pico_stdlib
    fs_lib
    microsd_driver
)

# Enable USB output for test
pico_enable_stdio_usb(automated_streaming_test 1)
pico_enable_stdio_uart(automated_streaming_test 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(automated_streaming_test)
