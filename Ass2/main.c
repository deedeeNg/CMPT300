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

int main(int argc, char** args) {

    printf("Starting..\n");
    List* receive_list = List_create();
    List* send_list = List_create();

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET; // Connect may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY); //Host to network long
    sin.sin_port = htons(PORT); //Host to network short

    // Create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socket to the port(PORT) that we specify
    bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    struct sockaddr_in sinRemote;
    unsigned int sin_len = sizeof(sinRemote);
    
    // Start up my modules
    Receiver_init("User 1", receive_list, socketDescriptor, sinRemote);
    Read_init(receive_list, "User 1");

    Write_init(send_list);
    Send_init("User_2",send_list, socketDescriptor, sinRemote);

    //Wait for user input
    printf("Enter something to kill the receive thread\n");
    char x;
    scanf("%c", &x);

    // Shutdown my modules
    Receiver_shutdown();
    Read_shutdown();
    Write_shutdown();
    Send_shutdown();

    printf("DONE..\n");

    return 0;
}