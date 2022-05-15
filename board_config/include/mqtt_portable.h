#ifndef MQTT_PORTABLE_H
#define MQTT_PORTABLE_H

#include "stdint.h"

#define MQTT_POLL_DELAY 10

void initMqtt(uint32_t priority, uint8_t* mqttConnected);
void publishMsg(const char* topic, uint8_t qos, uint8_t* payload, uint8_t* mqttPublished);

#endif // MQTT_PORTABLE_H