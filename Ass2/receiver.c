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

#define MSG_MAX_LEN 1024
#define PORT 22110

static pthread_t threadPID;
static int socketDescriptor;
static char* s_rxMessage;

pthread_mutex_t s_syncOkToReceiveMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t s_syncOkToReceiveCond = PTHREAD_COND_INITIALIZER;
struct sockaddr_in sinRemote;

void* receiveThread(List* receive_list) {
    while(1) {
        char messageRx[MSG_MAX_LEN];
        unsigned int sin_len = sizeof(sinRemote);
        int byteRx = recvfrom(socketDescriptor,
            messageRx, MSG_MAX_LEN, 0, 
            (struct sockaddr*) &sinRemote, &sin_len);

        int terminateIdx = (byteRx < MSG_MAX_LEN) ? byteRx : MSG_MAX_LEN - 1;
        messageRx[terminateIdx] = 0;

        char* message = (char*)malloc(strlen(messageRx));
        strcpy(message, messageRx);
        List_append(receive_list, message);
        pthread_mutex_lock(&s_syncOkToReceiveMutex);
        {
            pthread_cond_signal(&s_syncOkToReceiveCond);
        }
        pthread_mutex_unlock(&s_syncOkToReceiveMutex);
    }
}

void Receiver_init(char* rxMessage, List* receive_list, int socket, struct sockaddr_in remote) {
    s_rxMessage = rxMessage;
    socketDescriptor = socket;
    sinRemote = remote;
    pthread_create(
        &threadPID,             // PID(by pointer)
        NULL,                   // Attributes
        receiveThread,        // Function
        receive_list                  // Arguments
    );
}

void Receiver_shutdown(void) {
    // Cancel thread
    pthread_cancel(threadPID);

    // Waiting for thread to finish
    pthread_join(threadPID, NULL);
}