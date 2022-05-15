#!/bin/bash
#

USER_DIR=/home/$USER
PROJECT_DIR=$USER_DIR/magisterka-rtos

# FREERTOS
echo "### Start building app for FreeRTOS ###"
FREERTOS_DIR=$PROJECT_DIR/freertos
FREERTOS_BULD_DIR=$FREERTOS_DIR/build

if [ ! -d "$FREERTOS_BULD_DIR" ]; then
    mkdir $FREERTOS_BULD_DIR
fi

cd $FREERTOS_BULD_DIR
cmake $FREERTOS_DIR
make || { return; }
cd $PROJECT_DIR

# ZEPHYR
echo "### Start building app for Zephyr ###"
ZEPHYR_DIR=$PROJECT_DIR/zephyrproject/zephyr

cd $ZEPHYR_DIR
west build ./app || { return; }
cd $PROJECT_DIR

# Mbed OS
echo "### Start building app for Mbed OS ###"
MBED_OS_DIR=$PROJECT_DIR/mbed-os-app
cd $MBED_OS_DIR
mbed toolchain GCC_ARM
mbed config -G GCC_ARM_PATH "${USER_DIR}/zephyr-sdk-0.13.2/arm-custom-eabi/bin"
mbed compile -v -m NUCLEO_F207ZG -t GCC_ARM || { return; }
cd $PROJECT_DIR