#ifndef NETIF_TD_API_HDR_H
#define NETIF_TD_API_HDR_H

#include "netif_td.h"
#include "dhcpd_api.h"

extern struct td_speed_t td_record_tx;
extern struct td_speed_t td_record_rx;
extern struct statisticsData *st_td;
extern struct netif * netif_td;
extern struct netif * netif_td_ip6;

extern u32_t td_tx_pkt_cnt;
extern u32_t td_rx_pkt_cnt;
extern s8_t pkgNum;
extern s16_t pkgTotalLen;

void lte_construct_data_for_lte(u8_t cid, u8_t  *dataAddr, u16_t dataLen);

u32_t td_get_tx_speed(void);
u32_t td_get_rx_speed(void);
int netif_td_send_data(char *buf, int buf_len, int cid);
err_t lte_input_ip(void* data);
u8_t* lte_ul_buf_alloc(u32_t size, BOOL isQosPacket);
void lte_ul_buf_free(pmsg *buf);
void lte_tx_list_out(void);
void netif_td_init(void);
void lwip_set_netif_tune(lwip_ip_configinfo *ip_info);
void lwip_set_pdpinfo(lwip_ip_configinfo *ip_info);
void lwip_set_scid_table(char pCid, char sCid);
void lwip_set_scid_table_dsim(char simid, char pCid, char sCid);
void init_scid_table(void);
char get_pcid_from_scid_table(char simid, char scid);
struct pbuf * lte_dl_pbuf_reshape_with_cpy(struct pbuf * p, int p_free_flag);
struct pbuf * lte_ul_pbuf_reshape_with_cpy(struct pbuf * p);
struct pbuf * lte_dl_pbuf_reshape_noncpy(struct pbuf * p, int p_free_flag);
struct pbuf * lte_ul_pbuf_reshape_noncpy(struct pbuf * p);
struct pbuf* lte_pbuf_reshape_app_cpy(struct pbuf * p);

#endif
