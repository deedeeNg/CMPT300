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

static pthread_t threadPID;
static char* s_sxMessage;

struct addrinfo* sinSendRemote;

void* sendThread(List* send_list) {
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
        pthread_mutex_lock(&s_syncOkToSendMutex); 
        {
            pthread_cond_wait(&s_syncOkToSendCond, &s_syncOkToSendMutex);
        }
        pthread_mutex_unlock(&s_syncOkToSendMutex);
        char* messageSx = List_first(send_list);
        sendto(socketDescriptor,
            messageSx, strlen(messageSx), 0, 
            sinSendRemote->ai_addr, sinSendRemote->ai_addrlen);
        if (messageSx[0] == '!' && messageSx[2] == '\0') {
            shutDown = 0;
        } else {
            free(messageSx);
            List_remove(send_list);
        }
    }
}

void Send_init(char* sxMessage, List* send_list, struct addrinfo* remote) {
    s_sxMessage = sxMessage;
    sinSendRemote = remote;
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