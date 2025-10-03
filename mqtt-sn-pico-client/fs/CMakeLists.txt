# File System Library
add_library(fs_lib
    data_frame.c
)

target_include_directories(fs_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fs_lib
    pico_stdlib
    microsd_driver
)
