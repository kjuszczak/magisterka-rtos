#ifndef MQTT_PORTABLE_CPP_H
#define MQTT_PORTABLE_CPP_H

#include "stdint.h"

#define MQTT_POLL_DELAY 10

extern "C" void initMqttCpp(uint32_t priority, uint8_t* mqttConnected);
extern "C" void publishMsgCpp(const char* topic, uint8_t qos, uint8_t* payload, uint8_t* mqttPublished);

#endif // MQTT_PORTABLE_CPP_H