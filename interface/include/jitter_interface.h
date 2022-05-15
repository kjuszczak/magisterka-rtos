#ifndef JITTER_INTERFACE_H
#define JITTER_INTERFACE_H

#define TEST_ITERATION      1000

struct TestResults{
    uint32_t testTime[TEST_ITERATION + 1];
    uint32_t testIteration;
};

#endif // JITTER_INTERFACE_H
