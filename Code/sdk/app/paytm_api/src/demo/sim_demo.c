#include "paytm_dev_info_api.h"
#include "paytm_net_api.h"
#include "paytm_ntp_api.h"
#include "paytm_sim_api.h"
#include "paytm_typedef.h"

#include <stdlib.h>
#include <stdio.h>
#include "osi_api.h"

static int task_id = 0;
void simCb(void * p)
{
    Paytm_TRACE("This is sim cb, val = ");
}

void gprsCb(void * p)
{
    Paytm_TRACE("This is gprs cb, val = ");
}

void net_status_task(void* p)
{
    
}

void simMonitor(void * p)
{
    int result = *(int*)p;

    if(result == URC_SIM_REMOVED)
    {
        Paytm_TRACE("Sim card ejected");
        Paytm_SendMessage_From_ISR(task_id, 0, 55, 129);
    }else if(result == URC_SIM_INSERTED){
        Paytm_TRACE("Sim card inserted");
        Paytm_SendMessage_From_ISR(task_id, 1, 81, 366);
    }else if(result == URC_SIM_EJECTED_FOR_LONG_TIME){
        Paytm_TRACE("Sim card ejected for 15 minutes");
    }else if(result == URC_PDP_ACTIVE )
    {
        Paytm_TRACE("The pdp is active");
    }else if(result == URC_NET_ACTIVE ){
        Paytm_TRACE("The net connected successfully");
    }else if(result == URC_NET_DISCONNECTED ){
        Paytm_TRACE("The net is disconected");
    }else if(result == URC_SIM_READY){
        Paytm_TRACE("The sim card is ready");
    }
}

void testNetWorkReconected(void)
{
    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);

    network_state_callback_register(simMonitor);

    while (1)
    {
        /* code */
        if(Paytm_Net_IsConnected()){
            break;
        }
        Paytm_delayMilliSeconds(1000);
    }

    Paytm_TRACE("Paytm_GPRS_Disconnect");

    Paytm_GPRS_Disconnect();

    Paytm_delayMilliSeconds(1000);
    Paytm_TRACE("Paytm_GPRS_Reconnect");
    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);


    char op[32] = {0};
    Paytm_GetOperator(op, 32);
    Paytm_TRACE("op = %s", op);
}

void testNetWorkDisconected(void)
{
    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);

    network_state_callback_register(simMonitor);

    while (1)
    {
        /* code */
        if(Paytm_Net_IsConnected()){
            break;
        }
        Paytm_delayMilliSeconds(1000);
    }
    
    Paytm_TRACE("Paytm_GPRS_Disconnect");
    Paytm_GPRS_Disconnect();
}

static void msgTask0(void* p)
{
    ST_MSG msg = {0};

    while (1)
    {
        if(Paytm_GetMessage(task_id, &msg) == 0)
        {
            if(msg.message == 0){
                // Paytm_TRACE("Try to disconnect");
                // Paytm_GPRS_Disconnect();
                Paytm_delayMilliSeconds(5 * 1000);
            }else if(msg.message == 1)
            {
                // Paytm_TRACE("Try to connect");
                // Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);
            }
        }

        Paytm_delayMilliSeconds(10);
    }
    
}

void testSim(void)
{
    Paytm_Time gi = {0x00};

    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);

    Paytm_GetModemFunction((uint8 *)simMonitor);

    task_id = Paytm_CreateTask("1", msgTask0, NULL, 120, 4 * 1024);


    while (!Paytm_Net_IsConnected())
    {
        // Paytm_TRACE("Networking connecting, sim card status: %s", Paytm_GetSimState() ? "ready" : "not ready");
        Paytm_delayMilliSeconds(1000);
        Paytm_GetTime(&gi);
        Paytm_TRACE("%d-%d-%d, %d:%d:%d", gi.year, gi.month, gi.day, gi.hour, gi.minute, gi.second);
    }


}

void readSimState(void)
{
    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);

    Paytm_delayMilliSeconds(8000);

    int sim_state = Paytm_GetSimState();
    Paytm_TRACE("SIM state: %d", sim_state);
}

void readPDPInfo(void)
{
    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);

    Paytm_delayMilliSeconds(8000);

    char addr[16] = {0};
    Paytm_GetCGPADDR(addr);
    
    Paytm_TRACE("PDP info : %s", addr);
}

void readAPN(void)
{
    Paytm_GPRS_Connect(Paytm__IPVERSION_IPV4, NULL);

    Paytm_delayMilliSeconds(8000);

    char apn[64] = {0};
    PaytmGetAPN(apn);
    
    Paytm_TRACE("APN info : %s", apn);
}