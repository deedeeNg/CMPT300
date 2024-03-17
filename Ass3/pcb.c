#include "pcb.h"
#include "list.h"
#include <stdbool.h>
#include <stdio.h>


// Queues for processes's priority
List* high_priority;
List* medium_priority;
List* low_priority;

// List of blockers from send and receive operations
List* send_blocker;
List* receive_blocker;

// PCB and Semaphores
pcb* init_pcb;
pcb* curr_pcb;
List* list_pcb;
List* sem;

int pid = 0;

void create_pcb_init() {
    pcb* proc = malloc(sizeof((pcb)));
    proc->pid = 0;
    proc->priority = 2;
    proc->state = RUNNING;
    proc->proc_message = NULL;

    init_pcb = proc;

    //Initializes all the queues
    high_priority = ListCreate();
    medium_priority = ListCreate();
    low_priority = ListCreate();
    send_blocker = ListCreate();
    receive_blocker = ListCreate();
    list_pcb = ListCreate();
    sem = ListCreate();
}

void create_pcb(int priority) {
    pcb* proc = malloc(sizeof((pcb)));
    proc->pid = 0;
    proc->priority = 2;
    proc->state = READY;
    proc->proc_message = NULL;

    if (curr_pcb == NULL)
}
