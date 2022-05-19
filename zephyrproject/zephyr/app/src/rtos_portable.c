#include "rtos_portable.h"

#include <zephyr.h>
#include <soc.h>

#define STACK_SIZE              500
#define INITIAL_PRIORITY        0
/* INVERSE PRIORITY */
#define TASK_PRIORITY_BASE      3

TaskFunction tasks[MAX_NUMBER_OF_TASKS] = {NULL, NULL, NULL, NULL, NULL};

/* TASK 1 */
void zephyr_task_1(void *x, void *y, void *z)
{
    tasks[0](NULL);
}
K_THREAD_DEFINE(tid_task_1, STACK_SIZE,
                zephyr_task_1, NULL, NULL, NULL,
                INITIAL_PRIORITY, 0, -1);
/****************************************************************/

/* TASK 2 */
void zephyr_task_2(void *x, void *y, void *z)
{
    tasks[1](NULL);
}
K_THREAD_DEFINE(tid_task_2, STACK_SIZE,
                zephyr_task_2, NULL, NULL, NULL,
                INITIAL_PRIORITY, 0, -1);
/****************************************************************/

/* TASK 3 */
TaskFunction task_3;
void zephyr_task_3(void *x, void *y, void *z)
{
    tasks[2](NULL);
}
K_THREAD_DEFINE(tid_task_3, STACK_SIZE,
                zephyr_task_3, NULL, NULL, NULL,
                INITIAL_PRIORITY, 0, -1);
/****************************************************************/

/* TASK 4 */
TaskFunction task_4;
void zephyr_task_4(void *x, void *y, void *z)
{
    tasks[3](NULL);
}
K_THREAD_DEFINE(tid_task_4, STACK_SIZE,
                zephyr_task_4, NULL, NULL, NULL,
                INITIAL_PRIORITY, 0, -1);
/****************************************************************/

/* TASK 5 */
TaskFunction task_5;
void zephyr_task_5(void *x, void *y, void *z)
{
    tasks[4](NULL);
}
K_THREAD_DEFINE(tid_task_5, STACK_SIZE,
                zephyr_task_5, NULL, NULL, NULL,
                INITIAL_PRIORITY, 0, -1);
/****************************************************************/

const k_tid_t* task_tid[MAX_NUMBER_OF_TASKS] = {&tid_task_1, &tid_task_2, &tid_task_3, &tid_task_4, &tid_task_5};

struct k_sem zephyr_sem;

K_QUEUE_DEFINE(zephyr_queue);

/****************************************************/
K_THREAD_STACK_DEFINE(periodic_task_stack_area, STACK_SIZE);
struct k_work_q periodic_task_work_q;

struct k_work periodic_task_work[MAX_NUMBER_OF_PERIODIC_TASKS];
TaskFunction periodicTask[MAX_NUMBER_OF_PERIODIC_TASKS];

void periodic_task_handler_1(struct k_work *work)
{
    periodicTask[0](NULL);
}

void periodic_task_handler_2(struct k_work *work)
{
    periodicTask[1](NULL);
}

void timer_handler_1(struct k_timer *dummy)
{
    k_work_submit_to_queue(&periodic_task_work_q, &periodic_task_work[0]);
}
void timer_handler_2(struct k_timer *dummy)
{
    k_work_submit_to_queue(&periodic_task_work_q, &periodic_task_work[1]);
}

K_TIMER_DEFINE(timer_1, timer_handler_1, NULL);
K_TIMER_DEFINE(timer_2, timer_handler_2, NULL);

/****************************************************/

void createTask(TaskFunction task, const char *pcName, uint32_t priority, uint8_t taskIndex)
{
    (void) pcName;
    tasks[taskIndex] = task;
    k_thread_priority_set(*task_tid[taskIndex], TASK_PRIORITY_BASE - priority);
    k_thread_access_grant(*task_tid[taskIndex], &zephyr_queue);
    k_thread_start(*task_tid[taskIndex]);
}

void switchTask()
{
    k_yield();
}

void suspendTask(uint8_t taskIndex)
{
    k_thread_suspend(*task_tid[taskIndex]);
}

void resumeTask(uint8_t taskIndex)
{
    k_thread_resume(*task_tid[taskIndex]);
}

void resumeTaskFromIsr(uint8_t taskIndex)
{
    resumeTask(taskIndex);
}

uint8_t createSemaphore()
{
    return k_sem_init(&zephyr_sem, 1, 1) == 0;
}

uint8_t takeSemaphore()
{
    return k_sem_take(&zephyr_sem, K_FOREVER) == 0;
}

uint8_t giveSemaphore()
{
    k_sem_give(&zephyr_sem);
    return 1;
}

uint8_t createQueue(uint8_t queueSize)
{
    k_queue_init(&zephyr_queue);
    return 1;
}

uint8_t sendMsg(void* msg)
{
    k_queue_append(&zephyr_queue, msg);
    return 1;
}

uint8_t receiveMsg(void* msg)
{
    void* temp = k_queue_get(&zephyr_queue, K_FOREVER);
    if (temp != NULL)
    {
        *(uint8_t*)msg = *(uint8_t*)temp;
        return 1;
    }
    else
    {
        return 0;
    }
}

void createPeriodicTask(TaskFunction task, const char *pcName, uint16_t taskPeriod, uint32_t priority, uint8_t taskIndex)
{
    periodicTask[taskIndex] = task;

    if (taskIndex == 0)
    {
        k_work_q_start(&periodic_task_work_q, periodic_task_stack_area,
            K_THREAD_STACK_SIZEOF(periodic_task_stack_area), TASK_PRIORITY_BASE - priority);
        k_work_init(&periodic_task_work[taskIndex], periodic_task_handler_1);
        k_timer_start(&timer_1, K_MSEC(taskPeriod), K_MSEC(taskPeriod));
    }
    else
    {
        k_work_init(&periodic_task_work[taskIndex], periodic_task_handler_2);
        k_timer_start(&timer_2, K_MSEC(taskPeriod), K_MSEC(taskPeriod));
    }
}

void deletePeriodicTask(uint8_t taskIndex)
{
    if (taskIndex == 0)
    {
        k_timer_stop(&timer_1);
    }
    else
    {
        k_timer_stop(&timer_2);
    }
}

void sleepTask(uint32_t time)
{
    k_msleep(time);
}
