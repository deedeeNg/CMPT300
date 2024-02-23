#ifndef _WRITE_H_
#define _WRITE_H_

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include "write.h"
#include "send.h"
#include "receiver.h"
#include "shared.h"
#include "list.h"

void Write_init(List* send_list);
void Write_shutdown(void);

#endif