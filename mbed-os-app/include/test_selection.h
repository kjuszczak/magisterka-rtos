#ifndef TEST_SELECTION_H
#define TEST_SELECTION_H

/* Rhealstone benchmark */
#include "test_benchmark.h"

/* Jitter test */
#include "jitter_test.h"

/* MQTT test */
#include "connection_time.h"

void startRtosTest()
{
    startJitterTest();
    // startRhealstoneBenchmark();
    // testMqtt();
}

#endif // TEST_SELECTION_H
