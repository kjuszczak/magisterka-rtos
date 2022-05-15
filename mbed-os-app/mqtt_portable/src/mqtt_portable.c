#include "mqtt_portable.h"

void initMqttCpp(uint32_t priority, uint8_t* mqttConnected);
void publishMsgCpp(const char* topic, uint8_t qos, uint8_t* payload, uint8_t* mqttPublished);

void initMqtt(uint32_t priority, uint8_t* mqttConnected)
{
    initMqttCpp(priority, mqttConnected);
}

void publishMsg(const char* topic, uint8_t qos, uint8_t* payload, uint8_t* mqttPublished)
{
    publishMsgCpp(topic, qos, payload, mqttPublished);
}