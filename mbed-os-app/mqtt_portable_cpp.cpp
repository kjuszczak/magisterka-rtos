#include "mqtt_portable_cpp.h"

#include "mbed.h"

#include <MQTTClientMbedOs.h>
#include "EthernetInterface.h"

extern "C" {
#include "board_config.h"
}

#define MBED_OS_IP          "192.168.0.20"
#define MASK_IP             "255.255.255.0"
#define GW_IP               "192.168.0.30"
#define HOSTNAME            "192.168.0.30"
#define PORT                1883
#define MQTTVERSION_3_1_1   4
#define QUEUE_SIZE          10

static TCPSocket socket;
static EthernetInterface eth;
static MQTTClient client(&socket);
static MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

static Thread mqttThread;
static uint8_t* rtosMqttConnected = NULL;
struct mqtt_publish_info {
	const char* topic;
	MQTT::Message* msg;
    uint8_t* rtosMqttPublished;
};
static Queue<struct mqtt_publish_info, QUEUE_SIZE> mqtt_queue;

static void mqttSubHandler(MQTT::MessageData & msg);
static void mqttTask();
static void initIpStack();

void initMqttCpp(uint32_t priority, uint8_t* mqttConnected)
{
    rtosMqttConnected = mqttConnected;

    mqttThread.start(mqttTask);
    mqttThread.set_priority(static_cast<osPriority>(static_cast<uint32_t>(osPriorityNormal) + priority));
}

static void mqttTask()
{
    static struct mqtt_publish_info mqtt_received_info;
    static osEvent evt;

    initIpStack();

    data.MQTTVersion = MQTTVERSION_3_1_1;
    data.clientID.cstring = (char*)"mbed-mqtt";

    client.connect(data);
    *rtosMqttConnected = 1;

    for (;;) 
    {
        evt = mqtt_queue.get();
        if (evt.status == osEventMessage)
        {
            mqtt_received_info = *(struct mqtt_publish_info*)evt.value.p;
            client.publish(mqtt_received_info.topic, *mqtt_received_info.msg);
            *mqtt_received_info.rtosMqttPublished = 1;
        }
        client.yield(MQTT_POLL_DELAY);
    }
}

static void initIpStack()
{
    eth.set_network(MBED_OS_IP, MASK_IP, GW_IP);
    eth.connect();

    socket.open(&eth);
    socket.connect(HOSTNAME, PORT);
}

void publishMsgCpp(const char* topic, uint8_t qos, uint8_t* payload, uint8_t* mqttPublished)
{
    static struct mqtt_publish_info mqtt_sent_info;
    static MQTT::Message message;

    message.payload = payload;
    message.payloadlen = strlen((char*)payload);
    message.qos = (qos == 2) ? MQTT::QOS2 : (qos == 1) ? MQTT::QOS1 : MQTT::QOS0;
    message.retained = 0;

    mqtt_sent_info.topic = topic;
    mqtt_sent_info.msg = &message;
    mqtt_sent_info.rtosMqttPublished = mqttPublished;
    
    mqtt_queue.put(&mqtt_sent_info);
}