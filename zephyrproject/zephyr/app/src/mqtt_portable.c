#include "mqtt_portable.h"

#include "mqtt_config.h"

#include "board_config.h"

#include <zephyr.h>
#include <net/socket.h>
#include <net/mqtt.h>
#include <random/rand32.h>

#define APP_BMEM
#define APP_DMEM
#define STACK_SIZE 2048
#define THREAD_PRIORITY K_PRIO_PREEMPT(8)

/* INVERSE PRIORITY */
#define TASK_PRIORITY_BASE      3

/* Buffers for MQTT client. */
static APP_BMEM uint8_t rx_buffer[APP_MQTT_BUFFER_SIZE];
static APP_BMEM uint8_t tx_buffer[APP_MQTT_BUFFER_SIZE];

/* The mqtt client struct */
static APP_BMEM struct mqtt_client client_ctx;

/* MQTT Broker details. */
static APP_BMEM struct sockaddr_storage broker;

static APP_BMEM struct zsock_pollfd fds[1];
static APP_BMEM int nfds;

static APP_BMEM bool mqtt_connected;

static uint8_t subTopic[] = "rtos/zephyr/subscribe";
static struct mqtt_topic subs_topic;
static struct mqtt_subscription_list subs_list;

static uint8_t* rtosMqttConnected = NULL;
static uint8_t* rtosMqttPublishedQoS1 = NULL;
static uint8_t* rtosMqttPublishedQoS2 = NULL;

struct mqtt_publish_info {
	const char* topic;
	uint8_t qos;
	uint8_t* payload;
};
struct mqtt_publish_info mqtt_received_info = {
	.topic = NULL,
	.qos = 0,
	.payload = NULL,
};
K_QUEUE_DEFINE(mqtt_publish_queue);
/**/

static int start_app(void);

K_THREAD_DEFINE(app_thread, STACK_SIZE,
		start_app, NULL, NULL, NULL,
		THREAD_PRIORITY, K_USER, -1);
static K_HEAP_DEFINE(app_mem_pool, 1024 * 2);


static int publish();
/***********************************************/

static void prepare_fds(struct mqtt_client *client)
{
	if (client->transport.type == MQTT_TRANSPORT_NON_SECURE) 
    {
		fds[0].fd = client->transport.tcp.sock;
	}
#if defined(CONFIG_MQTT_LIB_TLS)
	else if (client->transport.type == MQTT_TRANSPORT_SECURE) 
    {
		fds[0].fd = client->transport.tls.sock;
	}
#endif

	fds[0].events = ZSOCK_POLLIN;
	nfds = 1;
}

static void clear_fds(void)
{
	nfds = 0;
}

void mqtt_evt_handler(struct mqtt_client *const client,
		      const struct mqtt_evt *evt)
{
	struct mqtt_puback_param puback;
	int len, err;

	switch (evt->type) {
		case MQTT_EVT_CONNACK:
			if (evt->result != 0) {
				print("MQTT connect failed %d\n", evt->result);
				break;
			}
			mqtt_connected = true;
			break;

		case MQTT_EVT_DISCONNECT:
			// print("MQTT client disconnected %d\n", evt->result);
			mqtt_connected = false;
			clear_fds();
			break;

		case MQTT_EVT_SUBACK:
			// print("SUBACK packet id: %u\n", evt->param.suback.message_id);
			break;

		case MQTT_EVT_UNSUBACK:
			// print("UNSUBACK packet id: %u\n", evt->param.suback.message_id);
			break;

		case MQTT_EVT_PUBACK:
			if (evt->result) {
				print("MQTT PUBACK error %d\n", evt->result);
				break;
			}
			*rtosMqttPublishedQoS1 = 1;
			break;

		case MQTT_EVT_PUBREC:
			if (evt->result != 0) {
				print("MQTT PUBREC error %d\n", evt->result);
				break;
			}

			const struct mqtt_pubrel_param rel_param = {
				.message_id = evt->param.pubrec.message_id
			};

			err = mqtt_publish_qos2_release(client, &rel_param);
			if (err != 0) {
				print("Failed to send MQTT PUBREL: %d\n", err);
			}

			break;

		case MQTT_EVT_PUBCOMP:
			if (evt->result != 0) {
				print("MQTT PUBCOMP error %d\n", evt->result);
				break;
			}
			*rtosMqttPublishedQoS2 = 1;
			break;

		case MQTT_EVT_PUBLISH:
			len = evt->param.publish.message.payload.len;
			print("MQTT publish received %d, %d bytes, id: %d, qos: %d\n",
				evt->result,
				len,
				evt->param.publish.message_id,
				evt->param.publish.message.topic.qos);
			break;

		default:
			print("DEFAULT: evt->type:%u\n", evt->type);
			break;
	}
}

static void subscribe(struct mqtt_client *client)
{
	int err;
	/* subscribe */
	subs_topic.topic.utf8 = subTopic;
	subs_topic.topic.size = strlen(subTopic);
	subs_list.list = &subs_topic;
	subs_list.list_count = 1U;
	subs_list.message_id = 1U;

	err = mqtt_subscribe(client, &subs_list);
	if (err) 
	{
		print("Failed on topic\n");
	}
}

static int publish()
{
	struct mqtt_publish_param param;

	param.message.topic.qos = (mqtt_received_info.qos == 2) ? MQTT_QOS_2_EXACTLY_ONCE :
		(mqtt_received_info.qos == 1) ? MQTT_QOS_1_AT_LEAST_ONCE : MQTT_QOS_0_AT_MOST_ONCE;;
	param.message.topic.topic.utf8 = mqtt_received_info.topic;
	param.message.topic.topic.size = strlen(mqtt_received_info.topic);
	param.message.payload.data = mqtt_received_info.payload;
	param.message.payload.len =
			strlen(param.message.payload.data);
	param.message_id = sys_rand32_get();
	param.dup_flag = 0U;
	param.retain_flag = 0U;

	return mqtt_publish(&client_ctx, &param);
}

static void poll_mqtt(void)
{
	if (mqtt_connected)
	{
		mqtt_input(&client_ctx);
	}
}

static void broker_init(void)
{
	struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker;

	broker4->sin_family = AF_INET;
	broker4->sin_port = htons(SERVER_PORT);
	zsock_inet_pton(AF_INET, SERVER_ADDR, &broker4->sin_addr);
}

static void client_init(struct mqtt_client *client)
{
    mqtt_client_init(client);

    broker_init();

    /* MQTT client configuration */
	client->broker = &broker;
	client->evt_cb = mqtt_evt_handler;
	client->client_id.utf8 = (uint8_t *)MQTT_CLIENTID;
	client->client_id.size = strlen(MQTT_CLIENTID);
	client->password = NULL;
	client->user_name = NULL;
	client->protocol_version = MQTT_VERSION_3_1_1;

    /* MQTT buffers configuration */
	client->rx_buf = rx_buffer;
	client->rx_buf_size = sizeof(rx_buffer);
	client->tx_buf = tx_buffer;
	client->tx_buf_size = sizeof(tx_buffer);

    client->transport.type = MQTT_TRANSPORT_NON_SECURE;
}

static int try_to_connect(struct mqtt_client *client)
{
    int rc, i = 0;
    while (i++ < APP_CONNECT_TRIES && !mqtt_connected)
    {
        client_init(client);

        rc = mqtt_connect(client);
		if (rc != 0) 
		{
			delay(APP_SLEEP_MSECS);
			continue;
		}

        prepare_fds(client);

		delay(APP_CONNECT_TIMEOUT_MS);
		mqtt_input(client);

		if (!mqtt_connected)
        {
			mqtt_abort(client);
		}
		else
		{
			*rtosMqttConnected = 1;
		}

		break;
    }

    if (mqtt_connected)
    {
		return 0;
	}

	return -EINVAL;
}

static int start_app(void)
{
	int rc = 0;
	void* mqttData;
	for (;;)
	{
		rc = 0;
		rc = try_to_connect(&client_ctx);
		if (rc)
		{
			print("Error with mqtt app\n");
			return rc;
		}

		mqttData = k_queue_get(&mqtt_publish_queue, K_MSEC(MQTT_POLL_DELAY));
		if (mqttData != NULL)
		{
			mqtt_received_info = *(struct mqtt_publish_info*)mqttData;
			publish();
		}
		poll_mqtt();
	}
    return 0;
}

void initMqtt(uint32_t priority, uint8_t* mqttConnected)
{
    rtosMqttConnected = mqttConnected;

	k_queue_init(&mqtt_publish_queue);
	k_thread_access_grant(k_current_get(), &mqtt_publish_queue);
	k_thread_access_grant(app_thread, &mqtt_publish_queue);

	k_thread_heap_assign(app_thread, &app_mem_pool);
	k_thread_priority_set(app_thread, TASK_PRIORITY_BASE - priority);
	k_thread_start(app_thread);
}

void publishMsg(const char* topic, uint8_t qos, uint8_t* payload, uint8_t* mqttPublished)
{
	static struct mqtt_publish_info mqtt_sent_info;
	if (qos == 1)
	{
		rtosMqttPublishedQoS1 = mqttPublished;
	}
	else if (qos == 2)
	{
		rtosMqttPublishedQoS2 = mqttPublished;
	}

	mqtt_sent_info.topic = topic;
	mqtt_sent_info.qos = qos;
	mqtt_sent_info.payload = payload;
	k_queue_append(&mqtt_publish_queue, (void*)&mqtt_sent_info);
}
