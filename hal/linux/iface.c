#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <stdarg.h>
#include <stdint.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>		/* for ETH_P_ALL */
#include <unistd.h>
#include "utc_hal_log.h"
#include "utc_hal_timer.h"

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t
strlcpy(char *dst, const char *src, size_t siz)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) {
		do {
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

struct ieee80211_channel {
	uint16_t hw_value;
	uint16_t center_freq;
};

#define CHAN2G(_channel, _freq) {           \
    .hw_value       = (_channel),           \
    .center_freq        = (_freq),          \
}

struct ieee80211_channel channels[] = {
	CHAN2G(1, 2412),//96c
	CHAN2G(2, 2417),//971
	CHAN2G(3, 2422),//976
	CHAN2G(4, 2427),//97b
	CHAN2G(5, 2432),//980
	CHAN2G(6, 2437),//985
	CHAN2G(7, 2442),//98a
	CHAN2G(8, 2447),//98f
	CHAN2G(9, 2452),//994
	CHAN2G(10, 2457),//999
	CHAN2G(11, 2462),//99e
	CHAN2G(12, 2467),//9a3
	CHAN2G(13, 2472),//9a8
	CHAN2G(14, 2484),//9b4
};

static char *program_name = "jolin_smartconfig";

/* VARARGS */
static inline void error(const char *fmt, ...)
{
	va_list ap;

	(void)fprintf(stderr, "%s: ", program_name);
	va_start(ap, fmt);
	(void)vfprintf(stderr, fmt, ap);
	va_end(ap);
	if (*fmt) {
		fmt += strlen(fmt);
		if (fmt[-1] != '\n')
			(void)fputc('\n', stderr);
	}
	exit(1);
	/* NOTREACHED */
}

int iface_set_mode(int sock_fd, const char *device, int mode)
{
	struct iwreq ireq;

	memset(&ireq, 0, sizeof(ireq));
	strlcpy(ireq.ifr_ifrn.ifrn_name, device, sizeof ireq.ifr_ifrn.ifrn_name);
	ireq.u.mode = mode;
	if (ioctl(sock_fd, SIOCSIWMODE, &ireq) == -1) {
		fprintf(stderr, "SIOCSIWMODE: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int iface_get_mode(int sock_fd, const char *device)
{
	struct iwreq ireq;

	memset(&ireq, 0, sizeof(ireq));
	strlcpy(ireq.ifr_ifrn.ifrn_name, device, sizeof ireq.ifr_ifrn.ifrn_name);
	if (ioctl(sock_fd, SIOCGIWMODE, &ireq) == -1) {
		fprintf(stderr, "SIOCGIWMODE: %s\n", strerror(errno));
		return -1;
	}

	return ireq.u.mode;
}


static int iface_get_flags(int sock_fd, const char *device)
{
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
		fprintf(stderr, "%s: Can't get flags: %s\n", device, strerror(errno));
		return -1;
	}

	return ifr.ifr_flags;
}

static int iface_set_down(int sock_fd, const char *device)
{
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
		fprintf(stderr, "%s: Can't get flags: %s\n", device, strerror(errno));
		return -1;
	}

	if (ifr.ifr_flags & IFF_UP) {
		ifr.ifr_flags &= ~IFF_UP;
		if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
			fprintf(stderr, "%s: Can't set flags: %s\n", device, strerror(errno));
			return -1;
		}
	}

	return 0;
}

static int iface_set_up(int sock_fd, const char *device, int oldflags)
{
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
		fprintf(stderr, "%s: Can't get flags: %s\n", device, strerror(errno));
		return -1;
	}

	ifr.ifr_flags |= oldflags;
	ifr.ifr_flags |= IFF_UP;
	if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
		fprintf(stderr, "%s: Can't set flags: %s\n", device, strerror(errno));
		return -1;
	}

	return 0;
}

#define WIFI_MODE_TRY_MAX   5

int set_wifi_mode(int sock_fd, const char *device, int mode)
{
    int oldflags;
    int index = 0;
    char flag = 0;

	oldflags = iface_get_flags(sock_fd, device);
	if (oldflags < 0) {
		error("Can't get %s flags", device);
		return -1;
	}

    utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%s-%d sys tick: %d ms\r\n", __FUNCTION__, __LINE__, utc_hal_system_tick_current()); 

	if ((iface_set_down(sock_fd, device)) < 0) {
		error("Can't set %s down", device);
		return -1;
	}

    utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%s-%d sys tick: %d ms\r\n", __FUNCTION__, __LINE__, utc_hal_system_tick_current()); 

    for(index=0; index<WIFI_MODE_TRY_MAX; index++){        
    	if (iface_set_mode(sock_fd, device, mode) < 0) {
    		error("Can't set %s mode", device);
    		return -1;
    	}
    
        if(iface_get_mode(sock_fd, device) == mode){
            flag = 1;        
            break;
        }else{
            printf("%s-%d  set mode:%d - %d error!\n", __FUNCTION__, __LINE__, mode, iface_get_mode(sock_fd, device));
            flag = 0;
            utc_hal_delay_ms(TIM_1S);
        }
    }

    if(0 == flag)
    {
        close(sock_fd);
        printf("%s-%d  set mode:%d  - %d error!\n", __FUNCTION__, __LINE__, mode, index);
        return -1;
    }

    utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%s-%d sys tick: %d ms\r\n", __FUNCTION__, __LINE__, utc_hal_system_tick_current()); 
	if (iface_set_up(sock_fd, device, oldflags) < 0) {
		error("Can't set %s up", device);
		return -1;
	}
    utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_DEBUG, "%s-%d sys tick: %d ms\r\n", __FUNCTION__, __LINE__, utc_hal_system_tick_current()); 

	return 0;
}




int iface_set_monitor_mode(int sock_fd, const char *device)
{
    int ret = 0;
    
    ret = set_wifi_mode(sock_fd, device, IW_MODE_MONITOR);

    if(!ret)
    {
        utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "iface[%s] set monitor mode success\r\n", device);
    }

    return ret;
}

int iface_set_managed_mode(int sock_fd, const char *device)
{
    int ret = 0;
    
    ret = set_wifi_mode(sock_fd, device, IW_MODE_INFRA);

    if(!ret)
    {
        utc_hal_log(TRMOD_SMART_CONFIG, TRLEV_INFO, "iface[%s] set managed mode success\r\n", device);
    }

    return ret;
}

int iface_set_freq(int sock_fd, const char *device, int freq)
{
	struct iwreq iwr;
	int ret = 0;

	if (!device) {
		printf("%s: %s is null\n", __func__, device);
		return -1;
	}
//    DBG_LOG();

	/*printf("sock_fd:%d, device:%s, freq:%d\n", sock_fd, device, freq); */

	memset(&iwr, 0, sizeof(iwr));
	strlcpy(iwr.ifr_name, device, sizeof(iwr.ifr_name));
	iwr.u.freq.m = freq * 100000;
	iwr.u.freq.e = 1;

	if (ioctl(sock_fd, SIOCSIWFREQ, &iwr) < 0) {
//		perror("ioctl[SIOCSIWFREQ]");
        printf("%s-%d: ioctl[SIOCSIWFREQ], freq: %d\r\n", __FUNCTION__, __LINE__, freq);
		ret = -1;
	}
//    DBG_LOG();

	return ret;
}

int iface_set_freq_1_to_14(int sock_fd, const char *device)
{
	int ret = 0;
	static int index = 8;

	ret = iface_set_freq(sock_fd, device, channels[index].center_freq);
    printf("iface_set_freq_1_to_14:  %d\r\n", index);
	index++;
	index = ((index) % 14);

	return ret;
}

int iface_set_freq_index(int sock_fd, const char *device, int index)
{
	int ret = 0;

//    DBG_LOG();
	ret = iface_set_freq(sock_fd, device, channels[index].center_freq);    
//    DBG_LOG();

	return ret;
}

int iface_get_freq_index(int freq)
{
	int channel = 0;
    int index = 0;

    for(index=0; index<(sizeof(channels)/sizeof(struct ieee80211_channel)); index++)
    {
        if(freq ==  channels[index].center_freq)
        {
            channel = channels[index].hw_value;
            break;
        }
    }
    

	return channel;
}



int iface_socket_bind(int sock_fd, const char *device, int protocol)
{
	struct ifreq ifr;
	struct sockaddr_ll ll;

	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(sock_fd, SIOCGIFINDEX, &ifr) < 0) {
		fprintf(stderr, "%s: ioctl[SIOCGIFINDEX]: %s", __func__, strerror(errno));
		return -1;
	}

	memset(&ll, 0, sizeof(ll));
	ll.sll_family = PF_PACKET;
	ll.sll_ifindex = ifr.ifr_ifindex;
	ll.sll_protocol = protocol;
	if (bind(sock_fd, (struct sockaddr *)&ll, sizeof(ll)) < 0) {
		fprintf(stderr, "%s: bind %s %s\n", __func__, device, strerror(errno));
		return -1;
	}

	return 0;
}
