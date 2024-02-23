#ifndef _SEND_H_
#define _SEND_H_

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include "send.h"
#include "write.h"
#include "receiver.h"
#include "shared.h"
#include "list.h"

void Send_init(char* sxMessage, List* send_list, struct addrinfo* remote);
void Send_shutdown(void);


#endif