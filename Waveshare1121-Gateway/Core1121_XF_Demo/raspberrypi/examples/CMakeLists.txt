# examples/CMakeLists.txt
# Used to build a static library named examples

# Recursively gather all .c source files
file(GLOB_RECURSE EXAMPLES_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_cad/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_firmware_update/*.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_per/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_ping_pong/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_read/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_sigfox/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_spectral_scan/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_spetrum_display/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_tx_cw/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_tx_infinite_preamble/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_write/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_LoRaWAN/*.c
)

# Create a static library
add_library(examples STATIC ${EXAMPLES_SRC})

# Set include directories
target_include_directories(examples PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/.
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_cad
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_firmware_update 
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_per
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_ping_pong
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_read
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_sigfox
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_spectral_scan
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_spetrum_display
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_tx_cw
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_tx_infinite_preamble
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_write
    ${CMAKE_CURRENT_SOURCE_DIR}/lr1121_LoRaWAN
)
include_directories(../src/gpio)
include_directories(../src/spi)
include_directories(../src/lr1121)

# Link to Pico SDK modules (for use by applications that link this library)
target_link_libraries(examples PUBLIC
    pico_stdlib
    gpio
    spi
    lr1121
)
