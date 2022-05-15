# FreeRTOS
openocd -f board/st_nucleo_f2.cfg -c "program ./freertos/build/main.elf verify reset exit"

# Zephyr
cd zephyrproject/zephyr/ && west flash && cd ../../

# Mbed-OS
openocd -f board/st_nucleo_f2.cfg -c "program ./mbed-os-app/BUILD/NUCLEO_F207ZG/GCC_ARM/mbed-os-app.elf verify reset exit"

# Windows
usbipd wsl list
py -3.7 C:\Users\bezbe\wsl_start.py
