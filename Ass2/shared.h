#ifndef _SHARED_H_
#define _SHARED_H_

#include <pthread.h>

extern pthread_mutex_t s_syncOkToReceiveMutex;
extern pthread_cond_t s_syncOkToReceiveCond;
extern pthread_mutex_t s_syncOkToSendMutex;
extern pthread_cond_t s_syncOkToSendCond;



#endif