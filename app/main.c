#include<stdio.h>
#include<signal.h>
#include<sys/time.h>//itimerval结构体的定义
#include <stdarg.h>
#include "utc_log.h"
#include "utc_task_timer.h"
#include "utc_smart_config.h"

int main(int argc, char **argv)
{
    u8 ssid[32] = {0};
    u8 psk[64] = {0};
    u16 ssid_len = 0;
    u16 psk_len = 0;
    s32 ret = OS_OK;
    u32 time = 0;
    
    utc_log_open();   
    utc_log_set_default_level();
    utc_log_set_level(TRMOD_UART, TRLEV_INFO);
    utc_log_set_level(TRMOD_SMART_CONFIG, TRLEV_INFO);
    utc_smart_config_open("wlp3s0");
    
    utc_log(TRMOD_UART, TRLEV_INFO, "app start\r\n");

    time = utc_system_tick_current();

    while(1)
    {           
        utc_task_timer_process();

        ret = utc_smart_config_get_ap(ssid, &ssid_len, psk, &psk_len);
        if(OS_OK == ret)
        {
            utc_log(TRMOD_SMART_CONFIG, TRLEV_NONE, "ssid:%s  psk:%s\r\n", ssid, psk);
            break;
        }else if(ERROR_SMART_CONFIG_AP_TIMEOUT == ret)
        {
            utc_log(TRMOD_SMART_CONFIG, TRLEV_ERROR, "smart config get ap info timeout\r\n");
            break;
        }

        if(utc_system_tick_current()-time>=2000){
            time = utc_system_tick_current();
            printf("%s-%d, ret:%x\r\n", __FUNCTION__, __LINE__, ret);
        }
    }
    utc_smart_config_close("wlp3s0");
}
