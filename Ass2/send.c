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

#define MSG_MAX_LEN 1024
#define PORT 22110

static pthread_t threadPID;
static int socketDescriptor;
static char* s_sxMessage;

struct sockaddr_in sinRemote;


void* sendThread(List* send_list) {
    while(1) {
        pthread_mutex_lock(&s_syncOkToSendMutex); 
        {
            pthread_cond_wait(&s_syncOkToSendCond, &s_syncOkToSendMutex);
        }
        pthread_mutex_unlock(&s_syncOkToSendMutex);
        char* messageSx = List_first(send_list);
        char messageTx[MSG_MAX_LEN];
        sprintf(messageTx, "%s: %s", s_sxMessage, messageSx);

        unsigned int sin_len = sizeof(sinRemote);
        sendto(socketDescriptor,
            messageTx, strlen(messageTx), 0, 
            (struct sockaddr*) &sinRemote, sin_len);

        free(messageSx);
        List_remove(send_list);
    }
}

void Send_init(char* sxMessage, List* send_list, int socket, struct sockaddr_in remote) {
    s_sxMessage = sxMessage;
    socketDescriptor = socket;
    sinRemote = remote;
    pthread_create(
        &threadPID,             // PID(by pointer)
        NULL,                   // Attributes
        sendThread,        // Function
        send_list                  // Arguments
    );
}

void Send_shutdown(void) {
    // Cancel thread
    pthread_cancel(threadPID);

    // Waiting for thread to finish
    pthread_join(threadPID, NULL);
}