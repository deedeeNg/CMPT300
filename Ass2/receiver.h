#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include "receiver.h"
#include "send.h"
#include "write.h"
#include "shared.h"
#include "read.h"
#include "list.h"

// Start background recerve thread
void Receiver_init(char* rxMessage, List* receive_list, struct addrinfo** remote);

// Stop background receive thread and cleanup
void Receiver_shutdown(void);

#endif