#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory and run cmake + make
cd build
cmake ..
make -j 8
