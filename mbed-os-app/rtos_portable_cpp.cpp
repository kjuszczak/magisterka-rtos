#include "rtos_portable_cpp.h"

#include "mbed.h"

extern "C" {
#include "board_config.h"
}

#define QUEUE_SIZE      10
#define MBED_OS_1_MS    0.06f

Thread tasks[MAX_NUMBER_OF_TASKS];

#define TASK_FLAG (1UL << 0)
static EventFlags event_flags[MAX_NUMBER_OF_TASKS];

static Semaphore mbed_sem(1);

typedef uint8_t message_t;
static Queue<message_t, QUEUE_SIZE> mbed_queue;
static osEvent evt;

static Thread event_thread;
static EventQueue the_event_queue;
static TaskFunction periodicTask[MAX_NUMBER_OF_PERIODIC_TASKS];
static Ticker timer[MAX_NUMBER_OF_PERIODIC_TASKS];

void createTaskCpp(TaskFunction task, uint32_t priority, uint8_t taskIndex)
{
    tasks[taskIndex].start(task);
    tasks[taskIndex].set_priority(static_cast<osPriority>(static_cast<uint32_t>(osPriorityNormal) + priority));
}

void switchTaskCpp()
{
    ThisThread::yield();
}

void delayTaskCpp(uint32_t delay)
{
    Thread::wait(delay);
}

void suspendTaskCpp(uint8_t taskIndex)
{
    event_flags[taskIndex].wait_any(TASK_FLAG);
}

void resumeTaskCpp(uint8_t taskIndex)
{
    event_flags[taskIndex].set(TASK_FLAG);
}

void resumeTaskFromIsrCpp(uint8_t taskIndex)
{
    resumeTaskCpp(taskIndex);
}

uint8_t createSemaphoreCpp()
{
    return 1;
}

uint8_t takeSemaphoreCpp()
{
    mbed_sem.wait();
    return 1;
}

uint8_t giveSemaphoreCpp()
{
    return mbed_sem.release() == osOK;
}

uint8_t createQueueCpp(uint8_t queueSize)
{
    return 1;
}

uint8_t sendMsgCpp(void* msg)
{
    mbed_queue.put((message_t*)msg);
    return 1;
}

uint8_t receiveMsgCpp(void* msg)
{
    evt = mbed_queue.get();
    if (evt.status == osEventMessage) {
        *(message_t*)msg = *(message_t*)evt.value.p;
        return 1;
    }
    return 0;
}

void timerHandler_1()
{
    the_event_queue.call(periodicTask[0]);
}

void timerHandler_2()
{
    the_event_queue.call(periodicTask[1]);
}

void createPeriodicTaskCpp(TaskFunction task, uint16_t taskPeriod, uint32_t priority, uint8_t taskIndex)
{
    periodicTask[taskIndex] = task;

    if (taskIndex == 0)
    {
        timer[taskIndex].attach(&timerHandler_1, MBED_OS_1_MS * static_cast<float>(taskPeriod));
        event_thread.start(callback(&the_event_queue, &EventQueue::dispatch_forever));
        event_thread.set_priority(static_cast<osPriority>(static_cast<uint32_t>(osPriorityRealtime) + priority));
    }
    else
    {
        timer[taskIndex].attach(&timerHandler_2, MBED_OS_1_MS * static_cast<float>(taskPeriod));
    }
}

void deletePeriodicTaskCpp(uint8_t taskIndex)
{
    timer[taskIndex].detach();
}

void sleepTaskCpp(uint32_t time)
{
    ThisThread::sleep_for(time);
}