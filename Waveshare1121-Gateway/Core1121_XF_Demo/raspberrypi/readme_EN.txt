/*****************************************************************************
* | File       :   Readme_EN.txt
* | Author     :   Waveshare team
* | Function   :   Help with use
* | Info       :
*----------------
* | This version:   V1.0
* | Date       :   2025-06-27
* | Info       :   This is an English version of the usage document to help you get started quickly
******************************************************************************/

This file helps you use this example.
A brief description of how to use this project is provided here:

1. Basic Information:
This example has been verified on Raspberry Pi 4B;
This example has been tested with the Core1121-XF module. You can check the corresponding test examples in the examples\ directory.

2. Pin Connection:
You can find the pin mapping in \src\gpio\dev_gpio.h. It is also listed here again:
Core1121-XF   =>    RPI(BCM)
3.3V         ->     3.3V  
GND          ->     GND  
MOSI         ->     10  
MISO         ->     9  
CLK          ->     11  
CS           ->     8  
BUSY         ->     24  
INT(DIO9)    ->     23  
RESET        ->     22  

3. Library Installation:
On Raspberry Pi  
    Install wiringPi  
        1.  git clone https://github.com/WiringPi/WiringPi  
        2.  cd WiringPi  
        3.  ./build  
        4.  gpio -v  
        5.  Running gpio -v should show version 2.70 or above. If not, the installation has failed.  

Raspberry Pi supports WIRINGPI.

4. Basic Usage:
The test programs can be found in the examples\ directory. In the Makefile, select the test example to use — only one can be selected at a time.
Run:  
    make clean  
    make  
    ./main
