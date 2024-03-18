#ifndef _PCB_H_
#define _PCB_H_

#include <stdio.h>
#include <stdbool.h>
#include "list.h"

enum States {
    READY,
    RUNNING,
    BLOCKED
};

typedef struct PCB_s PCB;
struct PCB_s {
    int pid;
    int priority;
    enum States state;
    char* proc_message;
};

// Queues for processes's priority
List* high_priority;
List* medium_priority;
List* low_priority;

// List of blockers from send and receive operations
List* send_blocker;
List* receive_blocker;

// PCB and Semaphores
PCB* init_pcb;
PCB* curr_pcb;
List* list_pcb;
List* sem;

bool compare_pid(void* pcb, void* pid);
void remove_pcb(int pid);
void next_pcb();
void create_pcb_init();
void create_pcb(int priority);
int fork_pcb();
void kill_pcb(int pid);
void total_info_pcb();

#endif

