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
static char* s_rxMessage;

struct addrinfo* sinReceiveRemote;

pthread_mutex_t s_syncOkToReceiveMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t s_syncOkToReceiveCond = PTHREAD_COND_INITIALIZER;

void* receiveThread(List* receive_list) {
    struct sockaddr_in sin;
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET; // Connect may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY); //Host to network long
    sin.sin_port = htons(local_port_int); //Host to network short

    // Create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socket to the port(PORT) that we specify
    bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    while(1) {
        char messageRx[MSG_MAX_LEN];
        struct sockaddr_in sinReceiveRemote;
        unsigned int sin_len = sizeof(sinReceiveRemote);
        int byteRx = recvfrom(socketDescriptor,
            messageRx, MSG_MAX_LEN, 0, 
            (struct sockaddr*) &sinReceiveRemote, &sin_len);

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

void Receiver_init(char* rxMessage, List* receive_list, struct addrinfo** remote) {
    s_rxMessage = rxMessage;
    sinReceiveRemote = remote;
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