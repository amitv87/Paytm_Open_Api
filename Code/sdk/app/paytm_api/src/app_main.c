#include <stdlib.h>
#include <stdio.h>
#include "osi_api.h"

#include "paytm_button_api.h"
#include "paytm_http_api.h"
#include "paytm_mqtt_api.h"
#include "paytm_net_api.h"
#include "paytm_sys_api.h"
#include "paytm_file_api.h"
#include "paytm_sim_api.h"
#include "paytm_debug_uart_api.h"
#include "paytm_led_api.h"

void LogTest(void)
{
    uint8_t data[10] = {'a', '0', '8', 99, 46, 13};

    Paytm_Logs_Enable(false);

    Paytm_TRACE_BUFFER_WRITE(data, 10, 1);

    Paytm_TRACE_TAG_DATA("Demo2:", data, 10);

    Paytm_TRACE_HEX_BUFFER("DemoHex:", data, 10);

    Paytm_TRACE("This id is %d in %s", 10, "China");

    Paytm_TRACE_FUNC("Demo7", "appSendData:", "%s.%d", "No", 1);
    
    Paytm_TRACE_DATETIME("L1", "Demo8", "appTex:", "get %d", 10010);

    Paytm_TRACE_DATETIME_PAYTM("L7", "2023-09-23", "Demo9", "%d", 456);
}

void* buttoncb(void * p)
{
    RTI_LOG1("This is buttoncb, action is ");
    if(*(int*)p == STATE_BUTTON_DOUBLE_CLICK)
    {
        RTI_LOG("Double click");
    }else if(*(int*)p == STATE_BUTTON_SINGLE_CLICK)
    {
        RTI_LOG("Single click");
    }else if(*(int*)p == STATE_BUTTON_LONG_PRESS)
    {
        RTI_LOG("Long press");
    }
}
void ButtonTest(void)
{
    button_action_callback_register(buttoncb);

    Paytm_Button_events(true);
}

/**
 * 结果：
*/
#define HTTP_GET_URL    "http://www.kuaidi100.com/query?type=shunfeng&postid=SF1420349064432"
#define HTTP_POST_URL   "http://pay.chinainfosafe.com:6003"
#define HTTPS_POST_URL  "https://visit.chinainfosafe.com/platform-admin/other/device/verification"
static char *post_data = "Hello chinainfosafe";

void net_connect(void)
{
    int stat = 0;

    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);
    
    while (!(Paytm_GetGPRSState(&stat) == 1 || Paytm_GetGPRSState(&stat) == 5))
    {
        Paytm_delayMilliSeconds(1000);
    }
    
    Paytm_TRACE("Network connected!");
}

void testHttpGet(void * p)
{
    int rc = 0;
    char *get_url = HTTP_GET_URL;

    net_connect();

    http_request_t http = {0};
    http.response_buffer = (char*)Paytm_malloc(HTTP_BUF_SIZE);
    http.content = (char*)Paytm_malloc(HTTP_BUF_SIZE);

    rc = Paytm_HTTP_Initialise_GET(LOC_EXTER_MEM, &http, get_url, 0, NULL);
    if(rc < 0)
    {
        Paytm_TRACE("Get fail!");
    }

    Paytm_TRACE("Header: %s", http.content);
    Paytm_TRACE("rspBuf: %s", http.response_buffer);

    if(http.response_buffer != NULL)
    {
        Paytm_free(http.response_buffer);
    }
    
    if(http.content != NULL)
    {
        Paytm_free(http.content);
    }

    while (1)
    {
        osiThreadSleep(1000);
    }
    
}

void testHttpPost(void * p)
{
    int stat = 0, rc = 0;
    char *post_url = HTTP_POST_URL;

    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);
    
    while (!(Paytm_GetGPRSState(&stat) == 1 || Paytm_GetGPRSState(&stat) == 5))
    {
        Paytm_delayMilliSeconds(1000);
    }
    
    Paytm_TRACE("Network connected!");

    http_request_t http = {0};
    http.response_buffer = (char*)Paytm_malloc(HTTP_BUF_SIZE);
    http.content = (char*)Paytm_malloc(HTTP_BUF_SIZE);

    rc = Paytm_HTTP_Initialise_POST(&http, post_url, NULL, post_data, false);
    if(rc < 0)
    {
        Paytm_TRACE("Post fail!");
    }

    Paytm_TRACE("Header: %s", http.content);
    Paytm_TRACE("rspBuf: %s", http.response_buffer);

    if(http.response_buffer != NULL)
    {
        Paytm_free(http.response_buffer);
    }
    
    if(http.content != NULL)
    {
        Paytm_free(http.content);
    }

    while (1)
    {
        osiThreadSleep(1000);
    }
}

void testHttpSSLPost(void * p)
{
    int rc = 0;
    char *post_url = HTTPS_POST_URL;

    net_connect();

    http_request_t http = {0};
    http.response_buffer = (char*)Paytm_malloc(HTTP_BUF_SIZE);
    http.content = (char*)Paytm_malloc(HTTP_BUF_SIZE);

    rc = Paytm_HTTP_Initialise_POST(&http, post_url, NULL, post_data, true);
    if(rc < 0)
    {
        Paytm_TRACE("Post fail! Rc = %d", rc);
    }

    Paytm_TRACE("Header: %s", http.content);
    Paytm_TRACE("rspBuf: %s", http.response_buffer);

    if(http.response_buffer != NULL)
    {
        Paytm_free(http.response_buffer);
    }
    
    if(http.content != NULL)
    {
        Paytm_free(http.content);
    }

    while (1)
    {
        osiThreadSleep(1000);
    }
}

void * mqttCb(void * p)
{

}

#define DEMO_MQTT_HOST  "pay.chinainfosafe.com"
#define DEMO_MQTT_PORT			6883

#define DEMO_MQTT_PRODUCT_KEY 	"demo3"
#define DEMO_USER_NAME   		"china"
#define DEMO_USER_PWD 			"12345611"

#define DEMO_SUB_TOPIC			"publish/2"
#define DEMO_PUB_TOPIC          "demo/003"
#define DEMO_PUB_DATA			"publish list"

void testMqtt(void)
{
    int rc = 0;
    char * host = DEMO_MQTT_HOST;
    char * client_id = DEMO_MQTT_PRODUCT_KEY;
    char * username = DEMO_USER_NAME;
    char * password = DEMO_USER_PWD;

    net_connect();

    Paytm_mqtt_connect_Packet_t mqtt_packet = {0};
    ST_MQTT_topic_info_t topic_list = {0};
    Paytm_mqtt_publish_Packet_t publish = {0};
    
    mqtt_packet.host = host;
    mqtt_packet.port = DEMO_MQTT_PORT;
    mqtt_packet.client_id = client_id;
    mqtt_packet.username = username;
    mqtt_packet.password = password;
    mqtt_packet.enable_ssl = false;

    rc = Paytm_MQTT_Initialise(NULL, CERTIFICATE_NVRAM, &mqtt_packet);
    if(rc < 0)
    {
        Paytm_TRACE("Mqtt init fail %d!", rc);
        return;
    }

    rc = Paytm_MQTT_Open();
    if(rc != 0)
    {
        Paytm_TRACE("Mqtt socket open fail %d!", rc);
        if(rc == -29)
        {
            RTI_LOG("Mqtt already opened");
            Paytm_MQTT_Disconnect();
        }
        return;
    }   

    rc = Paytm_MQTT_Connect();
    if(rc != 0)
    {
        Paytm_TRACE("Mqtt socket connect fail %d!", rc);
        return;
    }

    topic_list.topic[0] = DEMO_SUB_TOPIC;
    topic_list.qos[0] = Paytm_QOS0_AT_MOST_ONECE;
    topic_list.count = 1;

    rc = Paytm_MQTT_Subscribe(&topic_list);
    if(rc != 0)
    {
        Paytm_TRACE("Mqtt subscribe fail 0x%x!", rc);
    }

    publish.messageId = 58;
    publish.topic = DEMO_PUB_TOPIC;
    publish.message = DEMO_PUB_DATA;
    publish.message_length = strlen(publish.message);
    publish.qos = Paytm_QOS0_AT_MOST_ONECE;
    publish.retain = false;

    rc = Paytm_MQTT_Publish(&publish);
    if(rc != 0)
    {
        Paytm_TRACE("Mqtt publish fail 0x%x!", rc);
    }

    while (1)
    {
        /* code */
        osiThreadSleep(1000);
    }
}

extern void sys_initialize(void);
void app_main(void)
{
    sys_initialize();
    Paytm_Uart_Init();
    testMqtt();
    while (1)
    {
        //LogTest();
        osiThreadSleep(1000);
    }

    return;
}