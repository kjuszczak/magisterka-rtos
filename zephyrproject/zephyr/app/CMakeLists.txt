# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(BOARD nucleo_f207zg)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(magisterka)


target_sources(app
               PRIVATE
               src/main.c
               ${PROJECT_SOURCE_DIR}/../../../board_config/src/board_config.c
               ${PROJECT_SOURCE_DIR}/../../../board_config/src/hal_msp.c
               ${PROJECT_SOURCE_DIR}/../../../board_config/src/hal_it.c
               ${PROJECT_SOURCE_DIR}/../../../rhealstone/src/task_switch.c
               ${PROJECT_SOURCE_DIR}/../../../rhealstone/src/task_preemption.c
               ${PROJECT_SOURCE_DIR}/../../../rhealstone/src/interrupt_latency.c
               ${PROJECT_SOURCE_DIR}/../../../rhealstone/src/semaphore_shuffle.c
               ${PROJECT_SOURCE_DIR}/../../../rhealstone/src/deadlock_breaking_time.c
               ${PROJECT_SOURCE_DIR}/../../../rhealstone/src/intertask_msg_latency.c
               ${PROJECT_SOURCE_DIR}/../../../jitter/src/scenario.c
               ${PROJECT_SOURCE_DIR}/../../../mqtt/src/connection_time.c
               src/rtos_portable.c
               src/mqtt_portable.c)
target_include_directories(app
                           PRIVATE
                           ${PROJECT_SOURCE_DIR}/../../../board_config/include
                           ${PROJECT_SOURCE_DIR}/../../../rhealstone/include
                           ${PROJECT_SOURCE_DIR}/../../../jitter/include
                           ${PROJECT_SOURCE_DIR}/../../../mqtt/include
                           ${PROJECT_SOURCE_DIR}/../../../interface/include
                           ${PROJECT_SOURCE_DIR}/../../../include
                           include)
