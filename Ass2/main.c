#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "receiver.h"
#include "send.h"
#include "write.h"
#include "shared.h"
#include "read.h"
#include "list.h"

#define MSG_MAX_LEN 1024
static char* local_port;
static char* remote_port;
static char* remote_ip;

int local_port_int;
int remote_port_int;

int main(int argc, char* args[]) {

    printf("Starting..\n");
    List* receive_list = List_create();
    List* send_list = List_create();

    local_port = args[1];
    remote_ip = args[2];
    remote_port = args[3];

    local_port_int = atoi(local_port);
    remote_port_int = atoi(remote_port);

    struct addrinfo *remoteAddr;
    struct addrinfo hintRemote;

    memset(&hintRemote, 0, sizeof(hintRemote));
    hintRemote.ai_flags = 0;
	hintRemote.ai_family = AF_INET;
	hintRemote.ai_socktype = SOCK_DGRAM;
	hintRemote.ai_protocol = 0;
	hintRemote.ai_addrlen = 0;
	hintRemote.ai_canonname = NULL;
	hintRemote.ai_addr = NULL;
	hintRemote.ai_next = NULL;
    getaddrinfo(remote_ip, &remote_port[0], &hintRemote, &remoteAddr);
    
    // Start up my modules
    Receiver_init(receive_list);
    Read_init(receive_list, "New message: ");

    Write_init(send_list);
    Send_init("User 2: ",send_list, remoteAddr);

    //Wait for user input
    // printf("Enter something to kill the receive thread\n");
    // char x;
    // scanf("%c", &x);
    while (1) {
        
    }

    // Shutdown my modules
    Receiver_shutdown();
    Read_shutdown();
    Write_shutdown();
    Send_shutdown();

    printf("DONE..\n");

    return 0;
}