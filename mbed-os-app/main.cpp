/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#define NO_SYSTICK 1

#include "mbed.h"
// #include "stats_report.h"

extern "C" {
#include "board_config.h"
#include "test_selection.h"
}

Thread thread;

void LedBlinky_Task() {
    while (true) {
        // SystemReport sys_state(500 /* Loop delay time in ms */);
        print("Hello World from Mbed OS!\n");
        toggle_led();
        delay(60000);
        // Following the main thread_1 wait, report on the current system status
        // sys_state.report_state();
    }
}

// main() runs in its own thread_1 in the OS
int main()
{
    board_config();

    // print("MBed OS\n");

    // thread.start(LedBlinky_Task);

    startRtosTest();
}
