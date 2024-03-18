#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "pcb.h"

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

int pid = 0;

bool compare_pid(void* pcb, void* pid) {
    return ((PCB*)pcb)->pid == (int*)pid;
}

void remove_pcb(int pid) {

}
void next_pcb() {
    if (curr_pcb != 0) {
        curr_pcb->state = READY;
        if (curr_pcb->pid == 0) {
            List_append(high_priority, curr_pcb);
        } else if (curr_pcb->pid == 1) {
            List_append(medium_priority, curr_pcb);
        } else {
            List_append(low_priority, curr_pcb);
        }
    }

    // If there is a high, med or low priority pcb is waiting then move them to the running processor
    if (List_count(high_priority) > 0) {
        List_first(high_priority);
        curr_pcb = List_remove(high_priority);
        curr_pcb->state = RUNNING;
        printf("Current process switch to pid %d", curr_pcb->pid);
        return;
    } else if (List_count(medium_priority) > 0) {
        List_first(medium_priority);
        curr_pcb = List_remove(medium_priority);
        curr_pcb->state = RUNNING;
        printf("Current process switch to pid %d", curr_pcb->pid);
        return;
    } else if (List_count(low_priority) > 0) {
        List_first(low_priority);
        curr_pcb = List_remove(low_priority);
        curr_pcb->state = RUNNING;
        printf("Current process switch to pid %d", curr_pcb->pid);
        return;
    } else {
        curr_pcb = init_pcb;
        printf("Current process switch to init process");
        return;
    }
}

void create_pcb_init() {
    PCB* proc = malloc(sizeof(PCB));
    proc->pid = 0;
    proc->priority = 2;
    proc->state = RUNNING;
    proc->proc_message = NULL;

    init_pcb = proc;
    curr_pcb = init_pcb;

    //Initializes all the queues
    high_priority = List_create();
    medium_priority = List_create();
    low_priority = List_create();
    send_blocker = List_create();
    receive_blocker = List_create();
    list_pcb = List_create();
    sem = List_create();
}

void create_pcb(int priority) {
    pid++;
    PCB* proc = malloc(sizeof(PCB));
    proc->pid = pid;
    proc->priority = priority;
    proc->state = READY;
    proc->proc_message = NULL;

    if (curr_pcb == init_pcb) {
        proc->state = RUNNING;
    }

    // Adding to corresponding list
    List_append(list_pcb, proc);
    if (priority == 0) {
        List_append(high_priority, proc);
    } else if (priority == 1) {
        List_append(medium_priority, proc);
    } else {
        List_append(low_priority, proc);
    }
}

int fork_pcb() {
    if (curr_pcb == NULL) {
        printf("There is no process is running currently. Cannot fork!!\n");
        return -1;
    } else if (curr_pcb->pid == 0) {
        printf("Trying to fork init processor. Cannot fork!!\n");
        return -1;
    }

    pid++;
    PCB* proc = malloc(sizeof(PCB));
    proc->pid = pid;
    proc->priority = curr_pcb->priority;
    proc->state = READY;
    proc->proc_message = curr_pcb->proc_message;

    // Adding to corresponding list
    List_append(list_pcb, proc);
    if (proc->priority == 0) {
        List_append(high_priority, proc);
    } else if (proc->priority == 1) {
        List_append(medium_priority, proc);
    } else {
        List_append(low_priority, proc);
    }

    return proc->pid;
}


void kill_pcb(int pid) {
    if (curr_pcb->pid == pid) {
        //exit_pcb();
        return;
    }

    if (pid == 0) {
        if (List_count(list_pcb) > 0) {
            printf("Trying to kill init processor while other processes still running. Cannot kill!!\n");
            return;
        } else {
            //terminate();
            return;
        }
    }

    // Running the next pcb
    next_pcb();

    // Remove from the pcb, priority, send, receive and semaphores lists
    remove_pcb(pid);
}

void total_info_pcb() {
    if (List_count(list_pcb) > 0) {
        printf("List of process in the system: \n\n");
        List_first(list_pcb);
        while(List_curr(list_pcb) != NULL) {
            PCB* pcb = List_curr(list_pcb);
            char* state;
            if (pcb->state == READY) {
                state = "READY";
            } else if (pcb->state == RUNNING) {
                state = "RUNNING";
            } else {
                state = "BLOCKED";
            }
            printf("%d - %d - %s", pcb->pid, pcb->priority, state);
        }
    } else {
        printf("There are no processes running in the system");
    }
}