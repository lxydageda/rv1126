#ifndef NETSERVE_H
#define NETSERV_H
#include "common.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#define PORT 6556
#define IP "192.168.100.75"  
int serve_set();
#endif // !NETSERVE_H

