#ifndef _READ_H_
#define _READ_H_

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
#include "list.h"

// Start background recerve thread
void Read_init(List* receive_list, char* rxMessage);

// Stop background receive thread and cleanup
void Read_shutdown(void);

#endif