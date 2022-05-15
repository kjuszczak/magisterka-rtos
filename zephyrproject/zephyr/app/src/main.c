/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <soc.h>

#include "board_config.h"

#include "test_selection.h"

#include "connection_time.h"

#include "hal_it.h"

#define MY_STACK_SIZE 500
#define MY_PRIORITY 3

void LedBlinky_Task(void *x, void *y, void *z)
{
    while (true)
    {
        print("Hello World from Zephyr!\r\n");
        toggle_led();
        delay(1000);
    }
}

// K_THREAD_DEFINE(tid_led, MY_STACK_SIZE,
//                 LedBlinky_Task, NULL, NULL, NULL,
//                 MY_PRIORITY, 0, 0);

void main(void)
{
    IRQ_CONNECT(DMA1_Stream1_IRQn, 0, DMA1_Stream1_IRQHandler, 0, 0);
    IRQ_CONNECT(DMA1_Stream3_IRQn, 0, DMA1_Stream3_IRQHandler, 0, 0);
    IRQ_CONNECT(USART3_IRQn, 0, USART3_IRQHandler, 0, 0);
    IRQ_CONNECT(EXTI15_10_IRQn, 0, EXTI15_10_IRQHandler, 0, 0);

    board_config();

    print("Zephyr OS\n");

    // startRtosTest();

    testMqtt();
}
