#ifndef JITTER_SCENARIO_H
#define JITTER_SCENARIO_H

#include "stdint.h"

void startJitterTestScenario(uint8_t taskPriority_1, uint8_t taskPriority_2, uint8_t periodicTaskPriority, const char* testScenarioName);
void addSecondPeriodicTask(uint8_t periodicTaskPriority);
void printJitterTestResults();

#endif // JITTER_SCENARIO_H
