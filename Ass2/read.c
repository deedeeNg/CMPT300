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
#include "read.h"

#define MSG_MAX_LEN 1024
#define PORT 22110

static pthread_t threadPID;
static char* s_rxMessage;

void* readThread(List* receive_list) {
    while(1) {
        pthread_mutex_lock(&s_syncOkToReceiveMutex);
        {
            pthread_cond_wait(&s_syncOkToReceiveCond, &s_syncOkToReceiveMutex);
        }
        pthread_mutex_unlock(&s_syncOkToReceiveMutex); 
        char* message = List_first(receive_list);
        printf("%s: %s", s_rxMessage, message);
        fflush(stdout);
        free(message);
        List_remove(receive_list);
    }
}

void Read_init(List* receive_list, char* rxMessage) {
    s_rxMessage = rxMessage;
    pthread_create(
        &threadPID,             // PID(by pointer)
        NULL,                   // Attributes
        readThread,        // Function
        receive_list                  // Arguments
    );
}

void Read_shutdown(void) {
    // Cancel thread
    pthread_cancel(threadPID);

    // Waiting for thread to finish
    pthread_join(threadPID, NULL);
}