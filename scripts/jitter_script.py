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

means_rtos = []

def getTestResult(rtos, scenario):    
    ser = serial.Serial('/dev/ttyACM0', 115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)
    uart_data = ser.read(4000)
    data = []
    for i in range(0, 4000, 4):
        data.append(int.from_bytes([uart_data[i], uart_data[i+1], uart_data[i+2], uart_data[i+3]], byteorder="little"))

    plot_data = []
    for i in range(0, 1000 - 1):
        # plot_data.append(((data[i+1] - data[i]) / 60000))
        plot_data.append(((data[i+1] - data[i])))

    means_rtos.append(statistics.mean(plot_data))

    # open the file in the write mode
    f = open('/mnt/c/Users/bezbe/Documents/jitter_popatrzymy/' + 'hist_jitter_' + scenario + '_' + rtos + '.csv', 'w')

    # create the csv writer
    writer = csv.writer(f)

    # write a row to the csv file
    writer.writerow(plot_data)

    # close the file
    f.close()

    # plt.hist(plot_data, bins=20)
    # plt.title('Jitter - ' + rtos + " - " + scenario)
    # plt.grid()
    # plt.ylabel('Liczba próbek')
    # plt.xlabel('Czas [ms]')
    # plt.savefig('/mnt/c/Users/bezbe/Documents/jitter_popatrzymy/' + 'hist_jitter_' + scenario + '_' + rtos + '.png')
    # plt.close()

    # plt.plot(plot_data, 'o')
    # plt.title('Jitter - ' + rtos + " - " + scenario)
    # plt.grid()
    # plt.ylabel('Czas [ms]')
    # plt.xlabel('Numer iteracji')
    # plt.savefig('/mnt/c/Users/bezbe/Documents/jitter_popatrzymy/' + 'plot_jitter_' + scenario + '_' + rtos + '.png')
    # plt.close()

scenario_name = 'Scenario_3'

os.system(cmd_flash_freertos)
getTestResult('FreeRTOS', scenario_name)

os.system(cmd_flash_zephyr)
getTestResult('Zephyr', scenario_name)

os.system(cmd_flash_mbed_os)
getTestResult('MBed-OS', scenario_name)

print(means_rtos)