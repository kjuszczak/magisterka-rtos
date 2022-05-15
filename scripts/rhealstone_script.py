import os
import serial
import matplotlib.pyplot as plt
import csv

cmd_build = '. ./build_apps.sh'

cmd_flash_freertos = 'openocd -f board/st_nucleo_f2.cfg -c "program ./freertos/build/main.elf verify reset exit"'
cmd_copy_freertos_files = "cp ./scripts/rtos_files/rhealstone/FreeRTOSConfig.h ./freertos/include/FreeRTOSConfig.h"

cmd_flash_zephyr = 'cd zephyrproject/zephyr/ && west flash && cd ../../'
cmd_copy_zephyr_files = "cp ./scripts/rtos_files/rhealstone/prj.conf ./zephyrproject/zephyr/app/prj.conf"

cmd_flash_mbed_os = 'openocd -f board/st_nucleo_f2.cfg -c "program ./mbed-os-app/BUILD/NUCLEO_F207ZG/GCC_ARM/mbed-os-app.elf verify reset exit"'

# os.system(cmd_copy_freertos_files)
# os.system(cmd_copy_zephyr_files)
os.system(cmd_build)

def getTestResult(data, ser):    
    uart_data = ser.read(4)
    for i in range(0, 4, 4):
        data.append(int.from_bytes([uart_data[i], uart_data[i+1], uart_data[i+2], uart_data[i+3]], byteorder="little"))

def plotData(rtos, scenario, data):
    plot_data = []
    for d in data:
        # plot_data.append((d / 60) / 10000)
        plot_data.append(d)

    # open the file in the write mode
    f = open('/mnt/c/Users/bezbe/Documents/rhealstone_popatrzymy/' + 'hist_rhealstone_' + scenario + '_' + rtos + '.csv', 'w')

    # create the csv writer
    writer = csv.writer(f)

    # write a row to the csv file
    writer.writerow(plot_data)

    # close the file
    f.close()

    # print(plot_data)
    # plt.hist(plot_data[0], bins=5, label='FreeRTOS')
    # plt.hist(plot_data[1], bins=5, label='Zephyr')
    # plt.hist(plot_data[2], bins=5, label='MBed OS')
    # plt.legend()
    # plt.title(scenario)
    # plt.grid()
    # # plt.ylabel('Liczba próbek')
    # plt.xlabel('Czas [us]')
    # plt.savefig('/mnt/c/Users/bezbe/Documents/rhealstone/' + 'hist_rhealstone_' + scenario + '_' + rtos + '.png')
    # plt.close()


# scenario_name = 'Czas przesłania wiadomości pomiędzy zadaniami'
scenario_name = 'scenario_6'

data_freertos = []
data_zephyr = []
data_mbed_os = []

ser = serial.Serial('/dev/ttyACM0', 115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)

os.system(cmd_flash_freertos)
getTestResult(data_freertos, ser)
plotData('FreeRTOS', scenario_name, data_freertos)

os.system(cmd_flash_zephyr)
getTestResult(data_zephyr, ser)
plotData('Zephyr', scenario_name, data_zephyr)

os.system(cmd_flash_mbed_os)
getTestResult(data_mbed_os, ser)
plotData('MBed-OS', scenario_name, data_mbed_os)

# plotData('FreeRTOS', scenario_name, data)