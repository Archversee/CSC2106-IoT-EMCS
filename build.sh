#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project paths
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PICO_CLIENT_DIR="$PROJECT_DIR/mqtt-sn-pico-client"
BUILD_DIR="$PICO_CLIENT_DIR/build"
GATEWAY_DIR="$PROJECT_DIR/paho.mqtt-sn.embedded-c-master/MQTTSNGateway"

print_header() {
    echo -e "${BLUE}================================================${NC}"
    echo -e "${BLUE}  INF2004 Project Build Script${NC}"
    echo -e "${BLUE}  Pico W + MQTT-SN Gateway${NC}"
    echo -e "${BLUE}================================================${NC}"
}

print_usage() {
    echo -e "${YELLOW}Usage: $0 [COMMAND]${NC}"
    echo ""
    echo "Commands:"
    echo "  pico         - Build Pico W firmware (microSD demo)"
    echo "  gateway      - Build MQTT-SN Gateway (UDP) for laptop"
    echo "  clean        - Clean all build artifacts"
    echo "  flash        - Flash Pico W firmware via drag-and-drop"
    echo "  monitor      - Monitor Pico W serial output"
    echo "  setup        - Setup development environment"
    echo "  all          - Build everything (Pico W + Gateway)"
    echo "  help         - Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 pico      # Build only Pico W firmware"
    echo "  $0 gateway   # Build only MQTT-SN Gateway for laptop"
    echo "  $0 all       # Build both Pico W and Gateway"
    echo "  $0 flash     # Instructions for flashing Pico W"
}

check_pico_sdk() {
    if [ -z "$PICO_SDK_PATH" ]; then
        echo -e "${RED}PICO_SDK_PATH not set!${NC}"
        echo -e "${YELLOW}Please set PICO_SDK_PATH environment variable${NC}"
        echo -e "${YELLOW}Example: export PICO_SDK_PATH=/path/to/pico-sdk${NC}"
        return 1
    fi
    
    if [ ! -d "$PICO_SDK_PATH" ]; then
        echo -e "${RED}Pico SDK not found at: $PICO_SDK_PATH${NC}"
        return 1
    fi
    
    echo -e "${GREEN}Pico SDK found at: $PICO_SDK_PATH${NC}"
    return 0
}

setup_environment() {
    echo -e "${BLUE}Setting up development environment...${NC}"
    
    # Check if Pico SDK is available
    if ! check_pico_sdk; then
        echo -e "${YELLOW}Please install Pico SDK first:${NC}"
        echo "  git clone https://github.com/raspberrypi/pico-sdk.git"
        echo "  cd pico-sdk"
        echo "  git submodule update --init"
        echo "  export PICO_SDK_PATH=\$(pwd)"
        return 1
    fi
    
    # Check for required tools
    echo -e "${YELLOW}Checking required tools...${NC}"
    
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}cmake not found! Please install cmake${NC}"
        return 1
    fi
    
    if ! command -v make &> /dev/null; then
        echo -e "${RED}make not found! Please install build-essential${NC}"
        return 1
    fi
    
    echo -e "${GREEN}Development environment ready!${NC}"
}

build_pico() {
    echo -e "${BLUE}Building MQTT-SN Pico W client...${NC}"
    
    # Check Pico SDK
    if ! check_pico_sdk; then
        return 1
    fi
    
    # Create build directory if it doesn't exist
    if [ ! -d "$BUILD_DIR" ]; then
        mkdir -p "$BUILD_DIR"
        echo -e "${GREEN}Created build directory${NC}"
    fi
    
    cd "$BUILD_DIR"
    
    # Run cmake if CMakeCache.txt doesn't exist
    if [ ! -f "CMakeCache.txt" ]; then
        echo -e "${YELLOW}Running cmake configuration...${NC}"
        cmake .. || {
            echo -e "${RED}CMake configuration failed!${NC}"
            echo -e "${YELLOW}Make sure pico_sdk_import.cmake is in the mqtt-sn-pico-client directory${NC}"
            return 1
        }
    fi
    
    # Build the project
    echo -e "${YELLOW}Compiling MQTT-SN Pico W client...${NC}"
    make mqtt_sn_pico_client || {
        echo -e "${RED}Pico W build failed!${NC}"
        return 1
    }
    
    echo -e "${GREEN}MQTT-SN Pico W client built successfully!${NC}"
    echo -e "${GREEN}UF2 file: $BUILD_DIR/mqtt_sn_pico_client.uf2${NC}"
    echo -e "${YELLOW}To flash: Hold BOOTSEL button, connect Pico W, drag UF2 file to RPI-RP2 drive${NC}"
    
    cd "$PROJECT_DIR"
}

build_gateway() {
    echo -e "${BLUE}Building MQTT-SN Gateway for laptop...${NC}"
    
    if [ ! -d "$GATEWAY_DIR" ]; then
        echo -e "${RED}Gateway directory not found: $GATEWAY_DIR${NC}"
        echo -e "${YELLOW}Please ensure paho.mqtt-sn.embedded-c is in the project directory${NC}"
        echo -e "${YELLOW}You can clone it with:${NC}"
        echo "  git clone https://github.com/eclipse/paho.mqtt-sn.embedded-c.git"
        return 1
    fi
    
    cd "$GATEWAY_DIR"
    
    # Make build script executable
    chmod +x build.sh
    
    echo -e "${YELLOW}Running gateway build script for UDP...${NC}"
    ./build.sh udp || {
        echo -e "${RED}Gateway build failed!${NC}"
        return 1
    }
    
    echo -e "${GREEN}MQTT-SN Gateway built successfully!${NC}"
    echo -e "${GREEN}Gateway executable should be in: $GATEWAY_DIR/bin/${NC}"
    
    cd "$PROJECT_DIR"
}

clean_all() {
    echo -e "${BLUE}Cleaning build artifacts...${NC}"
    
    # Clean Pico build
    if [ -d "$BUILD_DIR" ]; then
        echo -e "${YELLOW}Removing Pico W build directory...${NC}"
        rm -rf "$BUILD_DIR"
    fi
    
    # Clean Gateway build
    if [ -d "$GATEWAY_DIR" ]; then
        cd "$GATEWAY_DIR"
        if [ -f "build.sh" ]; then
            echo -e "${YELLOW}Cleaning Gateway build...${NC}"
            ./build.sh clean 2>/dev/null || echo -e "${YELLOW}Gateway clean completed${NC}"
        fi
        cd "$PROJECT_DIR"
    fi
    
    echo -e "${GREEN}All build artifacts cleaned!${NC}"
}

flash_pico() {
    echo -e "${BLUE}Flashing MQTT-SN Pico W client...${NC}"
    
    UF2_FILE="$BUILD_DIR/mqtt_sn_pico_client.uf2"
    
    if [ ! -f "$UF2_FILE" ]; then
        echo -e "${RED}UF2 file not found! Build the project first.${NC}"
        echo -e "${YELLOW}Run: $0 pico${NC}"
        return 1
    fi
    
    echo -e "${YELLOW}To flash your Pico W:${NC}"
    echo "1. Hold down the BOOTSEL button on your Pico W"
    echo "2. Connect the Pico W to your computer via USB"
    echo "3. Release the BOOTSEL button"
    echo "4. A drive named 'RPI-RP2' should appear"
    echo "5. Drag and drop the following file to the RPI-RP2 drive:"
    echo -e "${GREEN}   $UF2_FILE${NC}"
    echo ""
    echo "The Pico W will automatically reboot and run your program!"
}

monitor_pico() {
    echo -e "${BLUE}Monitoring Pico W serial output...${NC}"
    echo -e "${YELLOW}Connect to your Pico W via serial terminal${NC}"
    echo "Common serial ports:"
    echo "  Linux: /dev/ttyUSB0 or /dev/ttyACM0"
    echo "  macOS: /dev/cu.usbmodem*"
    echo "  Windows: COM3, COM4, etc."
    echo ""
    echo "Settings: 115200 baud, 8N1"
    echo ""
    echo "Example commands:"
    echo "  screen /dev/ttyACM0 115200    (Linux/macOS)"
    echo "  minicom -D /dev/ttyACM0       (Linux)"
    echo "  cu -l /dev/cu.usbmodem* -s 115200  (macOS)"
}

# Main script logic
case "$1" in
    "pico")
        print_header
        build_pico
        ;;
    "gateway")
        print_header
        build_gateway
        ;;
    "clean")
        print_header
        clean_all
        ;;
    "flash")
        print_header
        flash_pico
        ;;
    "monitor")
        print_header
        monitor_pico
        ;;
    "setup")
        print_header
        setup_environment
        ;;
    "all")
        print_header
        echo -e "${BLUE}Building all components...${NC}"
        build_pico && build_gateway
        ;;
    "help"|"--help"|"-h")
        print_header
        print_usage
        ;;
    *)
        print_header
        echo -e "${RED}Unknown command: $1${NC}"
        echo ""
        print_usage
        exit 1
        ;;
esac
