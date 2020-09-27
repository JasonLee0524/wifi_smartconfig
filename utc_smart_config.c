/******************************************************************
 * 2019 (C)   UTCloud Copyright. All Rights Reserved.
 * File name    : utc_smart_config.c
 * Author       : 
 * Version      : V1.0
 * Date         : 2019.11.25
 * Description  :
 * Function List:
 * History      :
*******************************************************************/


/*---------------------------------- Include ---------------------------------*/
#include <string.h>
#include "cpack.h"
#include "extract.h"
#include "utc_log.h"
#include "utc_timer.h"
#include "utc_util.h"
#include "utc_task_timer.h"
#include "utc_smart_config.h"
#include "utc_hal_smart_config.h"

/*---------------------------------- Define  ---------------------------------*/
#define MCAST_ADDR1(p)  (p + 4)
#define MCAST_ADDR2(p)  (p + 10)
#define MCAST_ADDR3(p)  (p + 16)
#define MCAST_ADDR4(p)  (p + 24)

#define	BITNO_32(x) (((x) >> 16) ? 16 + BITNO_16((x) >> 16) : BITNO_16((x)))
#define	BITNO_16(x) (((x) >> 8) ? 8 + BITNO_8((x) >> 8) : BITNO_8((x)))
#define	BITNO_8(x) (((x) >> 4) ? 4 + BITNO_4((x) >> 4) : BITNO_4((x)))
#define	BITNO_4(x) (((x) >> 2) ? 2 + BITNO_2((x) >> 2) : BITNO_2((x)))
#define	BITNO_2(x) (((x) & 2) ? 1 : 0)

#define BIT(n)  (1U << n)
#define IS_EXTENDED(__p)    \
    (EXTRACT_LE_32BITS(__p) & BIT(IEEE80211_RADIOTAP_EXT)) != 0

#define MAX_SSID_LEN   32
#define MAX_PSK_LEN   64
#define MAX_MAC_ADDR_LEN   6
#define MAX_SLINK_MAC_LEN  (MAX_SSID_LEN + MAX_PSK_LEN)

#define T_MGMT 0x0				/* management */
#define T_CTRL 0x1				/* control */
#define T_DATA 0x2				/* data */
#define T_RESV 0x3				/* reserved */

/*
 *  * Bits in the frame control field.
 *   */
#define FC_VERSION(fc)      ((fc) & 0x3)
#define FC_TYPE(fc)         (((fc) >> 2) & 0x3)
#define FC_SUBTYPE(fc)      (((fc) >> 4) & 0xF)
#define FC_TO_DS(fc)        ((fc) & 0x0100)
#define FC_FROM_DS(fc)      ((fc) & 0x0200)
#define FC_MORE_FLAG(fc)    ((fc) & 0x0400)
#define FC_RETRY(fc)        ((fc) & 0x0800)
#define FC_POWER_MGMT(fc)   ((fc) & 0x1000)
#define FC_MORE_DATA(fc)    ((fc) & 0x2000)
#define FC_PROTECTED(fc)    ((fc) & 0x4000)
#define FC_ORDER(fc)        ((fc) & 0x8000)

/*---------------------------------- Variable Define -------------------------*/
typedef struct{
    u8 get_source_mac;
    u8 check_data_len;  //0-数据不全 1-数据完整
    u8 get_ssid_psk;
    u8 ssid_crc8;
    u8 psk_crc8;
    u8 from_source_mac[2][6];
	u8 ssid_psk_flag[MAX_SLINK_MAC_LEN];//recv data flag
    u8 ssid_mac[2*MAX_SSID_LEN+1];//ssid: bit0:len bit1-2*MAX_SSID_LEN: data
    u8 psk_mac[2*MAX_PSK_LEN+1];  //psk : bit0:len bit1-2*MAX_PSK_LEN : data
    u8 ssid[MAX_SSID_LEN];//recv ssid
    u8 psk[MAX_PSK_LEN];//recv psk
}smart_config_t;

typedef struct ieee80211_radiotap_header {
	u8 it_version;			/* Version 0. Only increases
								 * for drastic changes,
								 * introduction of compatible
								 * new fields does not count.
								 */
	u8 it_pad;
	u16 it_len;			/* length of the whole
								 * header in bytes, including
								 * it_version, it_pad,
								 * it_len, and data fields.
								 */
	u32 it_present;		/* A bitmap telling which
								 * fields are present. Set bit 31
								 * (0x80000000) to extend the
								 * bitmap by another 32 bits.
								 * Additional extensions are made
								 * by setting bit 31.
								 */
}ieee80211_radiotap_header_t;

typedef enum ieee80211_radiotap_type {
	IEEE80211_RADIOTAP_TSFT = 0,
	IEEE80211_RADIOTAP_FLAGS = 1,
	IEEE80211_RADIOTAP_RATE = 2,
	IEEE80211_RADIOTAP_CHANNEL = 3,
	IEEE80211_RADIOTAP_FHSS = 4,
	IEEE80211_RADIOTAP_DBM_ANTSIGNAL = 5,
	IEEE80211_RADIOTAP_DBM_ANTNOISE = 6,
	IEEE80211_RADIOTAP_LOCK_QUALITY = 7,
	IEEE80211_RADIOTAP_TX_ATTENUATION = 8,
	IEEE80211_RADIOTAP_DB_TX_ATTENUATION = 9,
	IEEE80211_RADIOTAP_DBM_TX_POWER = 10,
	IEEE80211_RADIOTAP_ANTENNA = 11,
	IEEE80211_RADIOTAP_DB_ANTSIGNAL = 12,
	IEEE80211_RADIOTAP_DB_ANTNOISE = 13,
	IEEE80211_RADIOTAP_RX_FLAGS = 14,
	IEEE80211_RADIOTAP_XCHANNEL = 18,
	IEEE80211_RADIOTAP_MCS = 19,
	IEEE80211_RADIOTAP_AMPDU_STATUS = 20,
	IEEE80211_RADIOTAP_VHT = 21,
	IEEE80211_RADIOTAP_NAMESPACE = 29,
	IEEE80211_RADIOTAP_VENDOR_NAMESPACE = 30,
	IEEE80211_RADIOTAP_EXT = 31
}ieee80211_radiotap_type_e;


static u8 g_channel_index = 5;//计数索引，比实际值小1
static p_task_timer_func_t p_sniffer_func;
task_timer_handle_t g_auto_switch_channel_timer;
static task_timer_handle_t g_smart_config_timeout_timer;
static task_timer_handle_t g_sniffer_timer;
static u8 *gp_device = NULL;
static s32 g_socket = 0;


static const u8 mcast_key0[] = { 0x01, 0x00, 0x5e, 0x5b, 0x55, 0x54 };
static const u8 mcast_key1[] = { 0x01, 0x00, 0x5e, 0x5c, 0x75, 0x74 };
static const u8 mcast_key2[] = { 0x01, 0x00, 0x5e, 0x63, 0x06, 0x08 };
static const u8 mcast_key3[] = { 0x01, 0x00, 0x5e, 0x62, 0x42, 0x27 };

static smart_config_t g_smart_config_data;
static u32 g_smart_config_timer = 0;

/*---------------------------------- Function declare ------------------------*/
static void smart_config_auto_switch_channel(void * param)
{
    s32 ret = OS_OK;

    
    if ((g_socket < 0) || (!gp_device)) {
        utc_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "%s %d device:%s\nn", __func__, __LINE__, gp_device);
        return;
    }

    utc_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "smart_config_auto_switch_channel: %d\r\n", g_channel_index+1);
    ret = utc_hal_smart_config_hal_switch_channel(g_channel_index);
    g_channel_index += 1;
    g_channel_index = (g_channel_index) % utc_hal_smart_config_hal_total_num_channel();
}

static void smart_config_stop(void * param)
{
    s32 ret = OS_OK;

    utc_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "smart_config_stop\r\n");  
    utc_task_timer_stop(g_auto_switch_channel_timer);
    utc_task_timer_delete(g_auto_switch_channel_timer);    
    utc_task_timer_stop(g_sniffer_timer);
    utc_task_timer_delete(g_sniffer_timer);    
    utc_task_timer_stop(g_smart_config_timeout_timer);
    utc_task_timer_delete(g_smart_config_timeout_timer); 
    utc_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "smart config timeout: %d ms\r\n", utc_system_tick_current()-g_smart_config_timer);
    g_smart_config_data.get_ssid_psk = 2;
    utc_hal_smart_config_close();
}

static void parse_ieee80211_radio_channel(u8 *p_packet_recv, u16 packet_recv_len, u16 *p_channel)
{
    u32 present_flags = (p_packet_recv[7]<<24) | (p_packet_recv[6]<<16) |\
                         (p_packet_recv[5]<<8) | (p_packet_recv[4]<<0);
    u32 present_flags1 = present_flags;
    u16 present_flags_index = 0;
    u16 channel_offset = 0;
    u16 freq = 0;

    *p_channel = 0;

    while(present_flags1 & 0x80000000)
    {
        present_flags_index ++;   
        present_flags1 = (p_packet_recv[7+present_flags_index*4]<<24) | (p_packet_recv[6+present_flags_index*4]<<16) |\
                         (p_packet_recv[5+present_flags_index*4]<<8) | (p_packet_recv[4+present_flags_index*4]<<0);
    }
//    printf("%s-%d: present_flags_index: %d\r\n", __FUNCTION__, __LINE__, present_flags_index);
    if(present_flags_index%2){
        present_flags_index++;
    }
    channel_offset = 4+(present_flags_index+1)*4;
//    printf("%s-%d: present_flags_index: %d-%d\r\n", __FUNCTION__, __LINE__, present_flags_index, channel_offset);
    if(present_flags & 0x00000001){
        channel_offset+=8;
    }
//    printf("%s-%d: present_flags_index: %d-%d\r\n", __FUNCTION__, __LINE__, present_flags_index, channel_offset);
//    if(present_flags & 0x00000002){
        channel_offset+=1;
//    }
//    printf("%s-%d: present_flags_index: %d-%d\r\n", __FUNCTION__, __LINE__, present_flags_index, channel_offset);
//    if(present_flags & 0x00000004){
        channel_offset+=1;
//    }
//    printf("%s-%d: present_flags_index: %d-%d\r\n", __FUNCTION__, __LINE__, present_flags_index, channel_offset);
    if(present_flags & 0x00000008){
        freq = (p_packet_recv[channel_offset+1]<<8) | (p_packet_recv[channel_offset]<<0);
        *p_channel = utc_hal_smart_config_freq_2_channel(freq);
        utc_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%s-%d: present_flags_index: %d-%d , freq: 0x%04x - channel: %d\r\n",\
                __FUNCTION__, __LINE__, present_flags_index, channel_offset, freq, *p_channel);
    }
}

static u16 parse_ieee80211_radio(u8 *p_packet_recv, u16 packet_recv_len, u16 *p_channel)
{
    u16 packet_head_len = 0;    
	struct cpack_state cpacker;
	const ieee80211_radiotap_header_t *hdr;
	u32 presentflags;
	const u32 *presentp;
	u8 flags;

	if (packet_recv_len < sizeof(*hdr)) {
		packet_head_len = packet_recv_len;
        goto out;
	}

	hdr = (const struct ieee80211_radiotap_header *)p_packet_recv;
	packet_head_len = EXTRACT_LE_16BITS(&hdr->it_len);

	if (packet_recv_len < packet_head_len) {
		packet_head_len = packet_recv_len;
        goto out;
	}

    parse_ieee80211_radio_channel(p_packet_recv, packet_recv_len, p_channel);

out:
    return packet_head_len;
}



static void check_from_source_mac(u16 channel)
{
	u8 *source0 = (u8 *) & g_smart_config_data.from_source_mac[0];
	u8 *source1 = (u8 *) & g_smart_config_data.from_source_mac[1];

	if (!g_smart_config_data.get_source_mac) {

		if ((source0[0] == 0) && (source0[1] == 0) &&
			(source0[2] == 0) && (source0[3] == 0) && (source0[4] == 0) && (source0[5] == 0))
			return;

		if ((source1[0] == 0) && (source1[1] == 0) &&
			(source1[2] == 0) && (source1[3] == 0) && (source1[4] == 0) && (source1[5] == 0))
			return;
        
		if (!memcmp(source0, source1, 6)) 
        {
			g_smart_config_data.get_source_mac = 1;

			utc_task_timer_stop(g_auto_switch_channel_timer);
            utc_delay_ms(TIM_1MS);
            if(g_channel_index != channel)
            {
                utc_hal_smart_config_hal_switch_channel(channel);
                utc_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "get source mac address, and reset channel: %d - %d\n", channel, g_channel_index+1);
            }
            else
            {
                utc_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "get source mac address, and channel: %d\n", channel);
            }            
		}
	}
}


static void parse_from_source_mac(u8 *p_mcaset, u8 *p_source, u16 channel)
{
    u8 *source0 = (u8 *) & g_smart_config_data.from_source_mac[0];
	u8 *source1 = (u8 *) & g_smart_config_data.from_source_mac[1];

    
    if (!memcmp(mcast_key0, p_mcaset, 6)) {
        utc_task_timer_reset(g_auto_switch_channel_timer);
        utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO,( s8 *) "mac mcast", ( s8 *)p_mcaset, 6);
		memcpy(source0, p_source, 6);
        utc_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "parse source mac address, and channel: %d\n", channel);
	}else if (!memcmp(mcast_key1, p_mcaset, 6)) {
        utc_task_timer_reset(g_auto_switch_channel_timer);
        utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, ( s8 *)"mac mcast",( s8 *) p_mcaset, 6);
		memcpy(source1, p_source, 6);
        utc_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "parse source mac address, and channel: %d\n", channel);
	}
}

static void parse_mcast_data_length(u8 *p_mcaset, u8 *p_source)
{   
    u8 *source0 = (u8 *) & g_smart_config_data.from_source_mac[0];

    if (!memcmp(source0, p_source, 6)) {
        if (p_mcaset[3] ==  mcast_key2[3])
        {
            if (!memcmp(mcast_key2, p_mcaset, 4)) 
            {
                if ((p_mcaset[4] <= MAX_SSID_LEN)
        			&& (p_mcaset[5] <= MAX_PSK_LEN)) 
        	    {
//                    utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, "len mcast", p_mcaset, 6);
        			g_smart_config_data.ssid_mac[0] = p_mcaset[4];
        			g_smart_config_data.psk_mac[0] = p_mcaset[5];
                    utc_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "ssid len:%d, psk len: %d\r\n",\
                        g_smart_config_data.ssid_mac[0], g_smart_config_data.psk_mac[0]);
        		}
    	    }
        }
    }
}

static void parse_ssid_psk(u8 *p_ssid, u16 ssid_len, u8 *p_psk,u16 psk_len)
{
    u8  dhh   = 0x00;
    u8  dhl   = 0x00;
    u8  dlh   = 0x00;
    u8  dll   = 0x00;
    u8  dh    = 0x00; 
    u8  dl    = 0x00;
    u16 index = 0x00;

    for(index=0; index<ssid_len; index++)
    {
        dhh = (p_ssid[index*2]>>4) & 0x0f;
        dhl = (p_ssid[index*2]) & 0x0f;
        dlh = (p_ssid[index*2+1]>>4) & 0x0f;
        dll = (p_ssid[index*2+1]) & 0x0f;

        dh = dhh ^ dhl;
        dl = dlh ^ dll;
        

        g_smart_config_data.ssid[index] = (dh<<4) | (dl);
        utc_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%02x-%02x dhh:%x, dhl:%x, dlh:%x, dll:%x, dh:%x, dl:%x ssid[%d]: %x-%c\r\n", 
            p_ssid[index*2], p_ssid[index*2+1], dhh, dhl, dlh, dll, dh, dl, index, g_smart_config_data.ssid[index], g_smart_config_data.ssid[index]);
    }

    for(index=0; index<psk_len; index++)
    {
        dhh = (p_psk[index*2]>>4) & 0x0f;
        dhl = (p_psk[index*2]) & 0x0f;
        dlh = (p_psk[index*2+1]>>4) & 0x0f;
        dll = (p_psk[index*2+1]) & 0x0f;

        dh = dhh ^ dhl;
        dl = dlh ^ dll;

        g_smart_config_data.psk[index] = (dh<<4) | (dl);
        utc_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%02x-%02x dhh:%x, dhl:%x, dlh:%x, dll:%x, dh:%x, dl:%xm psk[%d]: %x-%c\r\n", 
            p_psk[index*2], p_psk[index*2+1], dhh, dhl, dlh, dll, dh, dl, index, g_smart_config_data.psk[index-ssid_len], g_smart_config_data.psk[index-ssid_len]);
    }

    utc_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%s %d ssid:%s, psk:%s\n", __FUNCTION__, __LINE__, g_smart_config_data.ssid, g_smart_config_data.psk);
}

static void parse_mcast_data_crc8(u8 *p_mcaset, u8 *p_source)
{   
    u8 *source0 = (u8 *) & g_smart_config_data.from_source_mac[0];


    if(g_smart_config_data.check_data_len)
    {
        if (!memcmp(source0, p_source, 6)) {
            if (!memcmp(mcast_key3, p_mcaset, 4)) {
                u8 ssid_crc8 = 0;
                u8 psk_crc8 = 0;
            
                utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, (s8*)"crc8 mcast", (s8*)p_mcaset, 6);
                utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, (s8*)"crc8 source", (s8*)p_source, 6);
    			g_smart_config_data.ssid_crc8 = p_mcaset[4];
    			g_smart_config_data.psk_crc8 = p_mcaset[5];
                ssid_crc8 = utc_crc8_lsb(g_smart_config_data.ssid_mac, g_smart_config_data.ssid_mac[0]*2+1, ssid_crc8);
                psk_crc8 = utc_crc8_lsb(g_smart_config_data.psk_mac, g_smart_config_data.psk_mac[0]*2+1, psk_crc8);
                if(ssid_crc8 !=  p_mcaset[4] || psk_crc8 != p_mcaset[5])
                {
                    utc_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "ssid crc8:%x<%x>, psk crc8:%x<%x>\r\n", \
                        p_mcaset[4], ssid_crc8, p_mcaset[5], psk_crc8);
                    return;
                }
                
                parse_ssid_psk(&g_smart_config_data.ssid_mac[1], g_smart_config_data.ssid_mac[0], \
                               &g_smart_config_data.psk_mac[1], g_smart_config_data.psk_mac[0]);
                utc_task_timer_stop(g_sniffer_timer);
                utc_task_timer_delete(g_sniffer_timer);
                utc_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "smart config end: %d ms\r\n", utc_system_tick_current()-g_smart_config_timer);
                g_smart_config_data.get_ssid_psk = 1;                
                utc_hal_smart_config_close();
        	}
        }
    }else{
//        printf("%s-%d  %x\r\n", __FUNCTION__, __LINE__, g_smart_config_data.check_data_len);
    }
}


static void check_mcast_data_integrity(void)
{
	u16 index = 0;
    u16 count = 0;

    if(!g_smart_config_data.check_data_len){
    	int len = (g_smart_config_data.ssid_mac[0]+ g_smart_config_data.psk_mac[0]);
    	if (len > 0) {

            utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "ssid_psk_flag", g_smart_config_data.ssid_psk_flag, len);
            for (index = 0; index < len; index++){
    			if (g_smart_config_data.ssid_psk_flag[index]){
    				count++;
                }
            }
            if (count == len) 
            {
                utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_DEBUG, (s8*)"ssid_mac", (s8*)g_smart_config_data.ssid_mac, g_smart_config_data.ssid_mac[0]*2+1);
                utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_DEBUG, (s8*)"psk_mac", (s8*)g_smart_config_data.psk_mac, g_smart_config_data.psk_mac[0]*2+1);                
                g_smart_config_data.check_data_len = 1;
//                printf("%s-%d  %x\r\n", __FUNCTION__, __LINE__, g_smart_config_data.check_data_len);
    		}
    	}
    }
}

static void parse_mcast_ssid(u8 *p_mcaset, u8 *p_source)
{
    u8 *source0 = (u8 *) & g_smart_config_data.from_source_mac[0];
    
    if (!memcmp(source0, p_source, 6)) {
//        utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, "mcast1", (s8 *)p_mcaset, 6);
        if (p_mcaset[3] > MAX_SLINK_MAC_LEN)
				return;
        if (!memcmp(mcast_key2, p_mcaset, 3)) {
    		int index = p_mcaset[3];

            if (p_mcaset[3] > g_smart_config_data.ssid_mac[0])
    		    return;

            if(0 == g_smart_config_data.ssid_psk_flag[index-1] && \
                p_mcaset[3]<=g_smart_config_data.ssid_mac[0]){
                utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, (s8*)"ssid mcast", (s8*)p_mcaset, 6);
                g_smart_config_data.ssid_mac[2*(index-1)+2] = p_mcaset[4];
                g_smart_config_data.ssid_mac[2*(index-1)+1] = p_mcaset[5];
    			g_smart_config_data.ssid_psk_flag[(index-1)] = 1;
            }
		}
        check_mcast_data_integrity();
	}
}

static void parse_mcast_psk(u8 *p_mcaset, u8 *p_source)
{
    u8 *source0 = (u8 *) & g_smart_config_data.from_source_mac[0];
    
    if (!memcmp(source0, p_source, 6)) {
        if (p_mcaset[3] > MAX_SLINK_MAC_LEN)
				return;
        if (!memcmp(mcast_key2, p_mcaset, 3)) {
    		int index = p_mcaset[3];

            if (p_mcaset[3] > (g_smart_config_data.ssid_mac[0]+g_smart_config_data.psk_mac[0]))
    		    return;
            
            if(0 == g_smart_config_data.ssid_psk_flag[index-1] && \
                p_mcaset[3]>g_smart_config_data.ssid_mac[0] && \
                p_mcaset[3]<=(g_smart_config_data.psk_mac[0]+g_smart_config_data.ssid_mac[0]))
            {
                utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO,(s8*) "psk mcast", (s8*)p_mcaset, 6);
                g_smart_config_data.psk_mac[2*(index-g_smart_config_data.ssid_mac[0]-1)+2] = p_mcaset[4];
                g_smart_config_data.psk_mac[2*(index-g_smart_config_data.ssid_mac[0]-1)+1] = p_mcaset[5];
    			g_smart_config_data.ssid_psk_flag[(index-1)] = 1;

            }
		}
	}
    check_mcast_data_integrity();
}




static s32 parse_smart_config_frame(u16 fc, const u8 *p, u16 channel)
{    
	const u8 *mcast = NULL;
	const u8 *source = NULL;
    s32 ret = OS_OK;

	u8 *source0 = (u8 *) & g_smart_config_data.from_source_mac[0];
	u8 *source1 = (u8 *) & g_smart_config_data.from_source_mac[1];

	if (!FC_TO_DS(fc) && FC_FROM_DS(fc)) 
    {
		mcast  = MCAST_ADDR1(p);
		source = MCAST_ADDR3(p);
	}
    else if (FC_TO_DS(fc) && !FC_FROM_DS(fc)) 
    {
		mcast  = MCAST_ADDR3(p);
		source = MCAST_ADDR2(p);
	} 
    else
    {
		ret = -1;
        goto out;
    }
//    utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, "mcast1", (s8 *)mcast, 6);
//    utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_INFO, "source1", (s8 *)source, 6);

    if(g_smart_config_data.get_source_mac)
    {
        
        if(!g_smart_config_data.get_ssid_psk){
            parse_mcast_data_length((u8 *)mcast, (u8 *)source);
            parse_mcast_ssid((u8 *)mcast, (u8 *)source);
            parse_mcast_psk((u8 *)mcast, (u8 *)source);
            parse_mcast_data_crc8((u8 *)mcast, (u8 *)source);
        }
    }
    else
    {    
        parse_from_source_mac((u8 *)mcast, (u8 *)source, channel);      
        check_from_source_mac(channel);
    }
out:
    return ret;
}

static s32 parse_80211_packet_frame(u8* p, u16 len, u16 channel, s32 socket)
{
    u16 fc; 

    if(g_socket<0){
        g_socket = socket;
    }

   
    fc = EXTRACT_LE_16BITS(p);
    if (FC_TYPE(fc) == T_DATA) {
        parse_smart_config_frame(fc, p, channel);
        utc_log_hex(TRMOD_SMART_CONFIG, TRLEV_DEBUG, (s8*)"parse_80211_packet_frame", (s8*)p, len);            
    }
    return 0;
}


/*******************************************************************************
* Function    : utc_smart_config_open
* Description : 开启配网接口
* Input       : u8 *p_wifi_name: wifi name
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_smart_config_open(u8 *p_wifi_name)
{
    s32 ret = OS_OK;
    utc_task_timer_init();

    if(NULL == p_wifi_name)
    {
        ret = ERROR_SMART_CONFIG_WIFI_NAME;
        goto out;
    }

    g_smart_config_timer = utc_system_tick_current();
    utc_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "smart config start: %d ms\r\n", g_smart_config_timer);  
    ret = utc_hal_smart_config_open(p_wifi_name, parse_80211_packet_frame, &p_sniffer_func, parse_ieee80211_radio);
    if(NULL == g_auto_switch_channel_timer)
    {
        g_auto_switch_channel_timer = utc_task_timer_create(TIM_350MS, smart_config_auto_switch_channel, NULL);//350MS    
        utc_task_timer_start(g_auto_switch_channel_timer);
    }
    if(NULL == g_smart_config_timeout_timer)
    {
        g_smart_config_timeout_timer = utc_task_timer_create(TIM_30S, smart_config_stop, NULL);//30S
        utc_task_timer_start(g_smart_config_timeout_timer);
    }
    if(NULL == g_sniffer_timer && NULL != p_sniffer_func)
    {
        g_sniffer_timer = utc_task_timer_create(TIM_0MS, p_sniffer_func, NULL);//一直运行
        utc_task_timer_start(g_sniffer_timer);  
    } 
    utc_hal_smart_config_hal_switch_channel(g_channel_index);

    gp_device = p_wifi_name;

out:    
	return ret;
}

/*******************************************************************************
* Function    : utc_smart_config_close
* Description : 关闭配网接口
* Input       : u8 *p_wifi_name: wifi name
* Output      : none                
* Return      : OS_OK: 成功
*  			    other: 失败
* Others      : 
*******************************************************************************/
s32 utc_smart_config_close(u8 *p_wifi_name)
{
    s32 ret = OS_OK;
    utc_task_timer_init();

    if(NULL == p_wifi_name)
    {
        ret = ERROR_SMART_CONFIG_WIFI_NAME;
        goto out;
    }
    
    if(NULL != g_sniffer_timer && NULL != p_sniffer_func)
    {
        utc_task_timer_stop(g_sniffer_timer);
        utc_task_timer_delete(g_sniffer_timer);//一直运行  
        g_sniffer_timer = NULL;
    }  
    if(NULL != g_smart_config_timeout_timer)
    {
        utc_task_timer_stop(g_smart_config_timeout_timer);
        utc_task_timer_delete(g_smart_config_timeout_timer);//20S
        g_smart_config_timeout_timer = NULL;
    }   
    if(NULL != g_auto_switch_channel_timer)
    { 
        utc_task_timer_start(g_auto_switch_channel_timer);
        utc_task_timer_delete(g_auto_switch_channel_timer);//20S
        g_auto_switch_channel_timer = NULL;
    }  

    ret = utc_hal_smart_config_close();
    gp_device = NULL;

    
    g_channel_index = 5;//计数索引，比实际值小1
    g_socket = 0;
    memset(&g_smart_config_data, 0x00, sizeof(g_smart_config_data));

out:    
	return ret;
}


/*******************************************************************************
* Function    : utc_smart_config_get_ap
* Description : 获取配网结果接口
* Input       : none
* Output      : u8  *p_ssid     : ssid buf
*               u16 *p_ssid_len ：ssid len
*               u8  *p_psk      : psk  buf
*               u16 *p_psk_len  : psk  len                
* Return      : OS_OK                         : 配网成功
*  			    ERROR_SMART_CONFIG_AP_WAIT    ：等待配网结果
*               ERROR_SMART_CONFIG_AP_TIMEOUT : 配网超时
* Others      : 
*******************************************************************************/
s32 utc_smart_config_get_ap(u8 *p_ssid, u16 *p_ssid_len, u8 *p_psk, u16 *p_psk_len)
{
    s32 ret = OS_OK;
    
    if(NULL == p_ssid || NULL == p_ssid_len || NULL == p_psk || NULL == p_psk_len)
    {
        utc_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "[%s-%d] param is null\r\n", __FUNCTION__, __LINE__);
        ret = ERROR_SMART_CONFIG_AP_PARAM;
        goto out;
    }

    if(!g_smart_config_data.get_ssid_psk)
    {
        ret = ERROR_SMART_CONFIG_AP_WAIT;
        goto out;
    }

     if(2 == g_smart_config_data.get_ssid_psk)
    {
        ret = ERROR_SMART_CONFIG_AP_TIMEOUT;
        goto out;
    }
    
    *p_ssid_len = g_smart_config_data.ssid_mac[0];
    memcpy(p_ssid, g_smart_config_data.ssid, *p_ssid_len);
    *p_psk_len = g_smart_config_data.psk_mac[0];
    memcpy(p_psk, g_smart_config_data.psk, *p_psk_len);

out:
    return ret;
}
