#ifndef TEST_SELECTION_H
#define TEST_SELECTION_H

/* Rhealstone benchmark */
#include "test_benchmark.h"

/* Jitter test */
#include "jitter_test.h"

void startRtosTest()
{
    // startJitterTest();
    startRhealstoneBenchmark();
}

#endif // TEST_SELECTION_H
