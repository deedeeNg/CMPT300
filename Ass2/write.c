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

#define MSG_MAX_LEN 1024

pthread_mutex_t s_syncOkToSendMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t s_syncOkToSendCond = PTHREAD_COND_INITIALIZER;
List* send_list;

static pthread_t threadPID;

void* writeThread(List* send_list) {
    while(1) {
        char input[MSG_MAX_LEN];
        fgets(input, MSG_MAX_LEN, stdin);
        char* message = (char*)malloc(strlen(input) + 1);
        strcpy(message,input);
        List_append(send_list, message);
        pthread_mutex_lock(&s_syncOkToSendMutex);
        {
            pthread_cond_signal(&s_syncOkToSendCond);
        }
        pthread_mutex_unlock(&s_syncOkToSendMutex); 
    }
}

void Write_init(List* send_list) {
    pthread_create(
        &threadPID,             // PID(by pointer)
        NULL,                   // Attributes
        writeThread,        // Function
        send_list                  // Arguments
    );
}

void Write_shutdown(void) {
    // Cancel thread
    pthread_cancel(threadPID);

    // Waiting for thread to finish
    pthread_join(threadPID, NULL);
}