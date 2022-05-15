#ifndef TEST_BENCHMARK_H
#define TEST_BENCHMARK_H

/* Rhealstone benchmark */
#include "task_preemption.h"
#include "task_switch.h"
#include "interrupt_latency.h"
#include "semaphore_shuffle.h"
#include "deadlock_breaking_time.h"
#include "intertask_msg_latency.h"

void startRhealstoneBenchmark()
{
    /* Rhealstone benchmark */
    // startTaskSwitchTest();
    // startTaskPreemptionTest();
    // startInterruptLatencyTest();
    // startSemaphoreShuffleTest();
    // startDeadlockBreakingTimeTest();
    startIntertaskMsgLatencyTest();
}

#endif // TEST_BENCHMARK_H
