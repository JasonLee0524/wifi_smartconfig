#ifndef _IFACE_H
#define _IFACE_H

int iface_set_monitor_mode(int sock_fd, const char *device);
int iface_set_managed_mode(int sock_fd, const char *device);
int iface_set_freq_1_to_14(int sockfd, const char *device);
int iface_set_freq(int sockfd, const char *device, int freq);
int iface_socket_bind(int sock_fd, const char *device, int protocol);
int iface_set_freq_index(int sock_fd, const char *device, int index);
int iface_get_freq_index(int freq);
#endif
