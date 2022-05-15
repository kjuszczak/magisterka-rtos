#include "connection_time.h"

#include "board_config.h"
#include "rtos_portable.h"
#include "mqtt_portable.h"

#define MQTT_TASK       0
#define TEST_TASK       0
#define QOS0            0
#define QOS1            1
#define QOS2            2
#define TEST_ITERATION  100
#define PUBLISH_DELAY   500 //ms
#define RESET_DELAY     100 //ms

static uint8_t mqttConnected = 0;
static uint8_t mqttPublished = 0;
uint8_t mqttPayload[10] = {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE};
static uint32_t timerResults[TEST_ITERATION];

static void taskTestMqttConnection(void *pvParameters);
static void taskTestMqttPublicationQoS1(void *pvParameters);
static void taskTestMqttPublicationQoS2(void *pvParameters);
static void taskTestMqttPublication(uint8_t qos);
static void sendResultsViaUart();

void testMqtt()
{
    testMqttConnection();
    // testMqttPublicationQoS1();
    // testMqttPublicationQoS2();
};

void testMqttConnection()
{
    createTask(taskTestMqttConnection, "MQTT_Task", TEST_TASK, 0);
}

void testMqttPublicationQoS1()
{
    createTask(taskTestMqttPublicationQoS1, "MQTT_Task", TEST_TASK, 0);
}

void testMqttPublicationQoS2()
{
    createTask(taskTestMqttPublicationQoS2, "MQTT_Task", TEST_TASK, 0);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

static void taskTestMqttConnection(void *pvParameters)
{
    static uint32_t timerValue = 0;

    startTimer();
    initMqtt(MQTT_TASK, &mqttConnected);

    while (!mqttConnected) {}

    timerValue = getTimerValue();
    stopTimer();

    print("MQTT_timer");
    sleepTask(1);
    sendResults(&timerValue, 1);

    // sleepTask(RESET_DELAY);

    resetSystem();
    for (;;) {}
}

static void taskTestMqttPublicationQoS1(void *pvParameters)
{
    taskTestMqttPublication(QOS1);
}

static void taskTestMqttPublicationQoS2(void *pvParameters)
{
    taskTestMqttPublication(QOS2);
}

static void taskTestMqttPublication(uint8_t qos)
{
    initMqtt(MQTT_TASK, &mqttConnected);
    while (!mqttConnected) {}

    for (uint32_t i = 0; i < TEST_ITERATION; i++)
    {
        startTimer();
        mqttPublished = 0;
        publishMsg("agh/master/example", qos, mqttPayload, &mqttPublished);

        while (!mqttPublished) {}

        timerResults[i] = getTimerValue();
        stopTimer();

        sleepTask(PUBLISH_DELAY);
    }
    sendResultsViaUart();
}

static void sendResultsViaUart()
{
    print("MQTT_timer");
    sleepTask(1);
    sendResults(timerResults, TEST_ITERATION);
}

#pragma GCC pop_options
