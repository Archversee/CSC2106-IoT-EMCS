/*****************************************************************************
* | File      	:   Readme_CN.txt
* | Author      :   Waveshare team
* | Function    :   Help with use
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2025-06-27
* | Info        :   在这里提供一个中文版本的使用文档，以便你的快速使用
******************************************************************************/
这个文件是帮助您使用本例程。
在这里简略的描述本工程的使用：

1.基本信息：
本例程均在Raspberry Pi 4B 上进行了验证;
本例程使用 Core1121-XF 模块进行了验证，你可以在工程的examples\中查看对应的测试例程;

2.管脚连接：
管脚连接你可以在\src\gpio\dev_gpio.h中查看，这里也再重述一次：
Core1121-XF  =>    RPI(BCM)
3.3V         ->    3.3V
GND          ->    GND
MOSI         ->    10
MISO         ->    9
CLK          ->    11
CS           ->    8
BUSY         ->    24
INT(DIO9)    ->    23
RESET        ->    22


3.安装库：
Raspberry Pi
    安装wiringPi
        1.	git clone https://github.com/WiringPi/WiringPi
        2.	cd WiringPi
        3.	./build
        4.	gpio -v
        5.	运行gpio -v会出现2.70版本或更高，如果没有出现说明安装出错

Raspberry Pi 支持WIRINGPI


4.基本使用：
测试程序在examples\目录中查看，在Makefile文件中，选中测试示例，每次只能选中一个
Run:
    make clean 
    make
    ./main



