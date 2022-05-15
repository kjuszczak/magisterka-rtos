import os
import serial
import matplotlib.pyplot as plt
import statistics
import csv

cmd_build = '. ./build_apps.sh'

cmd_flash_freertos = 'openocd -f board/st_nucleo_f2.cfg -c "program ./freertos/build/main.elf verify reset exit"'
cmd_copy_freertos_files = "cp ./scripts/rtos_files/jitter/FreeRTOSConfig.h ./freertos/include/FreeRTOSConfig.h"

cmd_flash_zephyr = 'cd zephyrproject/zephyr/ && west flash && cd ../../'
cmd_copy_zephyr_files = "cp ./scripts/rtos_files/jitter/prj.conf ./zephyrproject/zephyr/app/prj.conf"

cmd_flash_mbed_os = 'openocd -f board/st_nucleo_f2.cfg -c "program ./mbed-os-app/BUILD/NUCLEO_F207ZG/GCC_ARM/mbed-os-app.elf verify reset exit"'

# os.system(cmd_copy_freertos_files)
# os.system(cmd_copy_zephyr_files)
os.system(cmd_build)

QOS = "QOS2"

ser = serial.Serial('/dev/ttyACM0', 115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)

timer_key = ['M', 'Q', 'T', 'T', '_', 't', 'i', 'm', 'e', 'r']

def getResults(mqtt_qos_time, cmd_flash):
    index = 0
    os.system(cmd_flash)
    uart_data = None
    while True:
        if index < len(timer_key):
            uart_data = ser.read(1)
            if int.from_bytes(uart_data, "big") == ord(timer_key[index]):
                index = index + 1
            else:
                index = 0
        else:
            uart_data = ser.read(400)
            break
    for i in range(0, 400, 4):
        mqtt_qos_time.append(int.from_bytes([uart_data[i], uart_data[i+1], uart_data[i+2], uart_data[i+3]], byteorder="little"))

def saveResults(file_name, data):
    # open the file in the write mode
    f = open('/mnt/c/Users/bezbe/Documents/mqtt_qos_time/' + QOS + '/' + file_name + '_' + QOS + '.csv', 'w')

    # create the csv writer
    writer = csv.writer(f)

    # write a row to the csv file
    writer.writerow(data)

    # close the file
    f.close()

freertos_results = []
getResults(freertos_results, cmd_flash_freertos)
saveResults("mqtt_freertos", freertos_results)

zephyr_results = []
getResults(zephyr_results, cmd_flash_zephyr)
saveResults("mqtt_zephyr",zephyr_results)

mbed_results = []
getResults(mbed_results, cmd_flash_mbed_os)
saveResults("mqtt_mbed",mbed_results)






