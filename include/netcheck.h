#ifndef NETCHECK_H
#define NETCHECK_H
#include "common.h"

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

typedef struct{
    int eth_connected;
    int wifi_connected;
    int can_path;
    pthread_mutex_t mutex;
}NetworkStatus;
#endif