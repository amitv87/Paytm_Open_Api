/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime: 2020-06-16 16:57:40
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "MQTTPacket.h"
#include "mqtt_list.h"
#include "platform_timer.h"
#include "platform_memory.h"
#include "platform_mutex.h"
#include "platform_thread.h"
#include "mqtt_defconfig.h"
#include "network.h"
#include "random.h"
#include "mqtt_error.h"
#include "mqtt_log.h"

/**
 * @brief               mqtt qos mode structure.
 */
typedef enum mqtt_qos {
    QOS0 = 0,                 ///<transport at most once
    QOS1 = 1,                 ///<transport at least once
    QOS2 = 2,                 ///<transport only once
    SUBFAIL = 0x80            ///<abandoned
} mqtt_qos_t;

/**
 * @brief               client state.
 */
typedef enum client_state {
	CLIENT_STATE_INVALID = -1,              ///<invalid 
	CLIENT_STATE_INITIALIZED = 0,           ///<intitialized
	CLIENT_STATE_CONNECTED = 1,             ///<connected
	CLIENT_STATE_DISCONNECTED = 2,          ///<disconnected
    CLIENT_STATE_CLEAN_SESSION = 3,         ///<session cleaned
	CLIENT_STATE_OPENED = 4                 ///<opened
}client_state_t;

/**
 * @brief               mqtt connack data structure.
 */
typedef struct mqtt_connack_data {
    uint8_t rc;                             ///<return code
    uint8_t session_present;                ///<present session flag
} mqtt_connack_data_t;

/**
 * @brief               mqtt message structure.
 */
typedef struct mqtt_message {
    mqtt_qos_t          qos;                ///<quality of service
    uint8_t             retained;           ///<retained flag
    uint8_t             dup;                ///<dup flag
    uint16_t            id;                 ///<message id
    size_t              payloadlen;         ///<payload length
    void                *payload;           ///<payload
} mqtt_message_t;

/**
 * @brief               message data structure.
 */
typedef struct message_data {
    char                topic_name[KAWAII_MQTT_TOPIC_LEN_MAX];          ///<topic name
    mqtt_message_t      *message;                                       ///<message structure
} message_data_t;

typedef void (*interceptor_handler_t)(void* client, message_data_t* msg);
typedef void (*message_handler_t)(void* client, message_data_t* msg);
typedef void (*reconnect_handler_t)(void* client, void* reconnect_date);

/**
 * @brief               message handle structure.
 */
typedef struct message_handlers {
    mqtt_list_t         list;                                           ///<mqtt client list
    mqtt_qos_t          qos;                                            ///<quality of service
    const char*         topic_filter;                                   ///<topic name
    message_handler_t   handler;                                        ///<handle of this message
} message_handlers_t;

/**
 * @brief               ack handle structure.
 */
typedef struct ack_handlers {
    mqtt_list_t         list;                                           ///<mqtt client list
    platform_timer_t    timer;                                          ///<platform timer
    uint32_t            type;                                           ///<ack type
    uint16_t            packet_id;                                      ///<ac>packet id
    message_handlers_t  *handler;                                       ///<handle of ack
    uint16_t            payload_len;                                    ///<payload length
    uint8_t             *payload;                                       ///<payload
} ack_handlers_t;

/**
 * @brief               mqtt client willoptions structure.
 */
typedef struct mqtt_will_options {
    mqtt_qos_t          will_qos;                                       ///<quality of service
    uint8_t             will_retained;                                  ///<retained message
    char                *will_topic;                                    ///<topic name
    char                *will_message;                                  ///<will message
} mqtt_will_options_t;

/**
 * @brief               mqtt client structure.
 */
typedef struct mqtt_client {
    char                        *mqtt_client_id;
    char                        *mqtt_user_name;
    char                        *mqtt_password;
    char                        *mqtt_host;
    char                        *mqtt_port;
    char                        *mqtt_ca;
    void                        *mqtt_reconnect_data;
    uint8_t                     *mqtt_read_buf;
    uint8_t                     *mqtt_write_buf;
    uint16_t                    mqtt_keep_alive_interval;
    uint16_t                    mqtt_packet_id;
    uint32_t                    mqtt_will_flag          : 1;
    uint32_t                    mqtt_clean_session      : 1;
    uint32_t                    mqtt_ping_outstanding   : 2;
    uint32_t                    mqtt_version            : 4;
    uint32_t                    mqtt_ack_handler_number : 24;
    uint32_t                    mqtt_cmd_timeout;
    uint32_t                    mqtt_read_buf_size;
    uint32_t                    mqtt_write_buf_size;
    uint32_t                    mqtt_reconnect_try_duration;
    size_t                      mqtt_client_id_len;
    size_t                      mqtt_user_name_len;
    size_t                      mqtt_password_len;
    mqtt_will_options_t         *mqtt_will_options;
    client_state_t              mqtt_client_state;
    platform_mutex_t            mqtt_write_lock;
    platform_mutex_t            mqtt_global_lock;
    mqtt_list_t                 mqtt_msg_handler_list;
    mqtt_list_t                 mqtt_ack_handler_list;
    network_t                   *mqtt_network;
    platform_thread_t           *mqtt_thread;
    platform_timer_t            mqtt_last_sent;
    platform_timer_t            mqtt_last_received;
    reconnect_handler_t         mqtt_reconnect_handler;
    interceptor_handler_t       mqtt_interceptor_handler;

	uint8_t 					mqtt_client_index;
	uint8_t 					mqtt_ssl_enable;
	uint8_t						mqtt_conn_return_code;
} mqtt_client_t;


#define KAWAII_MQTT_ROBUSTNESS_CHECK(item, err) if (!(item)) {                                         \
        KAWAII_MQTT_LOG_E("%s:%d %s()... check for error.", __FILE__, __LINE__, __FUNCTION__);         \
        return err; }

#define KAWAII_MQTT_CLIENT_SET_DEFINE(name, type, res)         \
    type mqtt_set_##name(mqtt_client_t *c, type t) {    \
        KAWAII_MQTT_ROBUSTNESS_CHECK((c), res);                \
        c->mqtt_##name = t;                             \
        return c->mqtt_##name;                          \
    }

#define KAWAII_MQTT_CLIENT_SET_STATEMENT(name, type)           \
    type mqtt_set_##name(mqtt_client_t *, type);

KAWAII_MQTT_CLIENT_SET_STATEMENT(client_id, char*)
KAWAII_MQTT_CLIENT_SET_STATEMENT(user_name, char*)
KAWAII_MQTT_CLIENT_SET_STATEMENT(password, char*)
KAWAII_MQTT_CLIENT_SET_STATEMENT(host, char*)
KAWAII_MQTT_CLIENT_SET_STATEMENT(port, char*)
KAWAII_MQTT_CLIENT_SET_STATEMENT(ca, char*)
KAWAII_MQTT_CLIENT_SET_STATEMENT(reconnect_data, void*)
KAWAII_MQTT_CLIENT_SET_STATEMENT(keep_alive_interval, uint16_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(will_flag, uint32_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(clean_session, uint32_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(version, uint32_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(cmd_timeout, uint32_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(read_buf_size, uint32_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(write_buf_size, uint32_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(reconnect_try_duration, uint32_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(reconnect_handler, reconnect_handler_t)
KAWAII_MQTT_CLIENT_SET_STATEMENT(interceptor_handler, interceptor_handler_t)

/**
 * @brief               sleeps mqtt function with milliseconds .
 * @param ms            time to push mqtt function into asleep.
 */
void mqtt_sleep_ms(uint32_t ms);

/**
 * @brief               initalize the mqtt client .
 * @return              handle of client.
 */
mqtt_client_t *mqtt_lease(void);

/**
 * @brief               release the mqtt client .
 * @param c             handles of client
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_release(mqtt_client_t* c);

/**
 * @brief               mqtt client connect to server.
 * @param c             handles of client
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_connect(mqtt_client_t* c);

/**
 * @brief               mqtt client disconnect to server.
 * @param c             handles of client
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_disconnect(mqtt_client_t* c);

/**
 * @brief               keep mqtt client alive.
 * @param c             handles of client
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_keep_alive(mqtt_client_t* c);

/**
 * @brief               subscribe topic.
 * @param c             handles of client
 * @param topic_filter  topic name
 * @param qos           Quality of Service
 * @param msg_handler   msg handle
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_subscribe(mqtt_client_t* c, const char* topic_filter, mqtt_qos_t qos, message_handler_t msg_handler);

/**
 * @brief               unsubscribe topic.
 * @param c             handles of client
 * @param topic_filter  topic name
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_unsubscribe(mqtt_client_t* c, const char* topic_filter);

/**
 * @brief               publish msg to topic.
 * @param c             handles of client
 * @param topic_filter  topic name
 * @param msg           msg handle
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_publish(mqtt_client_t* c, const char* topic_filter, mqtt_message_t* msg);

/**
 * @brief               lists all topic subscribed into mqtt_msg_handler_list.
 * @param c             handles of client
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_list_subscribe_topic(mqtt_client_t* c);

/**
 * @brief               set will options.
 * @param c             handles of client
 * @param topic         topic name
 * @param qos           Quality of Service
 * @param retained      messages retained
 * @param message       will message to send
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_set_will_options(mqtt_client_t* c, char *topic, mqtt_qos_t qos, uint8_t retained, char *message);

/**
 * @brief               open mqtt with results.
 * @param c             handles of client
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_open_with_results(mqtt_client_t* c);

/**
 * @brief               mqtt connects with results.
 * @param c             handles of client
 * @return              
 *          - 0         success
 *          - others    failure
 */
int mqtt_conn_with_results(mqtt_client_t* c);


/**
 * @brief               get mqtt client status.
 * @param c             handles of client
 * @return              
 *          - state     mqtt client status, elements in client_state_t
 *          - others    failure
 */
client_state_t paho_mqtt_get_state(mqtt_client_t* c);

/**
 * @brief               set mqtt client status.
 * @param c             handles of client
 * @param state         mqtt client status
 */
void paho_mqtt_set_state(mqtt_client_t* c, client_state_t state);

/**
 * @brief               clean mqtt session.
 * @param c             handles of client
 */
void mqtt_clean_session(mqtt_client_t* c);

uint32_t paho_mqtt_get_msg_addr(uint8_t index);
#endif /* _MQTTCLIENT_H_ */
