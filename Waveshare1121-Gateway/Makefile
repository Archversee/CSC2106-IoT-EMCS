# Makefile - Waveshare1121-Gateway
# Compiles main.c with the full Waveshare LR1121 driver stack
# Usage:
#   make        → build
#   make run    → build and run (needs sudo)
#   make clean  → remove binary

CC     = gcc
CFLAGS = -Wall -O2 -DUSE_WIRINGPI_LIB -DLR1121_PING_PONG
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/src
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/src/lr1121
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/src/lr1121/lr11xx_driver
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/src/lr1121/lr1121_modem
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/src/lr1121/lr1121_common
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/src/lr1121/lr1121_printers
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/src/spi
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/src/gpio
CFLAGS += -I./Core1121_XF_Demo/raspberrypi/examples
LIBS   = -lwiringPi -lm

DEMO_SRC := $(shell find Core1121_XF_Demo/raspberrypi/src -name "*.c")
EXAMPLE_SRC := Core1121_XF_Demo/raspberrypi/examples/lr1121_config.c

TARGET := lora_rx

.PHONY: all clean run

all: $(TARGET)

$(TARGET): main.c $(DEMO_SRC) $(EXAMPLE_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "Build OK → ./$(TARGET)"

clean:
	rm -f $(TARGET)

run: all
	sudo ./$(TARGET)