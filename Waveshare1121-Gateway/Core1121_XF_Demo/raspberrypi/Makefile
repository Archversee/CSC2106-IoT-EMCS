# ==== Compiler and Flags ====
CC = gcc

# Example selection (uncomment only one)
# EXAMPLES = -DLR1121_CAD

# EXAMPLES = -DLR1121_FIRMWARE_UPDATE -DLR1121_TRANSCEIVER_0103
# EXAMPLES = -DLR1121_FIRMWARE_UPDATE -DLR1121_MODEM_05020001

# EXAMPLES = -DLR1121_PER
# EXAMPLES = -DLR1121_PING_PONG            # Currently selected example
# EXAMPLES = -DLR1121_READ
# EXAMPLES = -DLR1121_WRITE
# EXAMPLES = -DLR1121_SIGFOX
# EXAMPLES = -DLR1121_SPECTRAL_SCAN
# EXAMPLES = -DLR1121_SPETRUM_DISPLAY
# EXAMPLES = -DLR1121_TX_CW
# EXAMPLES = -DLR1121_TX_INFINITE_PRERMBLE
# EXAMPLES = -DLR1121_LR_FHSS

EXAMPLES = -DLR1121_LORAWAN

# Compilation flags
CFLAGS = -Wall -O0 -g
CFLAGS += -DUSE_WIRINGPI_LIB $(EXAMPLES)
CFLAGS += -Isrc -Iexamples -I./src/lr1121 -I./src/lr1121/lr11xx_driver -I./src/spi -I./src/gpio

LIBS = -lwiringPi -lm

# ==== Directory Settings ====
DIR_SRC := src
DIR_EXAMPLES := examples
DIR_BIN := bin

SRC := $(shell find $(DIR_SRC) $(DIR_EXAMPLES) -name "*.c")
OBJ := $(patsubst %.c, $(DIR_BIN)/%.o, $(SRC))
TARGET := main

# ==== Color Codes ====
YELLOW = \033[1;33m
GREEN  = \033[1;32m
BLUE   = \033[1;34m
RESET  = \033[0m

# ==== Default Target ====
all: banner $(TARGET)

# ==== Build Banner ====
banner:
	@echo -e "$(BLUE)==> Building project: $(TARGET)$(RESET)"
	@echo -e "$(BLUE)==> Example Flags: $(EXAMPLES)$(RESET)"

# ==== Linking ====
$(TARGET): $(OBJ)
	@echo -e "$(YELLOW)[Linking]$(RESET) $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	@echo -e "$(GREEN)✔ Build complete: $(TARGET)$(RESET)"

# ==== Compilation Rule ====
$(DIR_BIN)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)[Compiling]$(RESET) $<"
	@$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

# ==== Clean ====
clean:
	@echo -e "$(BLUE)==> Cleaning project...$(RESET)"
	@rm -rf $(DIR_BIN) $(TARGET)
	@echo -e "$(GREEN)✔ Clean done.$(RESET)"
