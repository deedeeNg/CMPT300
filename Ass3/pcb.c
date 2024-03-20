#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "pcb.h"

int pid = 0;

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
List* list_sem;

bool compare_pid(void* pcb, void* pid) {
    return ((PCB*)pcb)->pid == *(int*)pid;
}

bool compare_sid(void* sem, void* sid) {
    return ((SEM*)sem)->sid == *(int*)sid;
}

void put_pcb(PCB* pcb) {
    if (pcb == NULL) {
        printf("There is no process to put it in waiting queue. Please try again!!\n");
        return;
    }
    if (pcb->priority == 0) {
        printf("Putting process pid %d to high priority queue... \n", pid);
        List_append(high_priority, pcb);
        return;
    } else if (pcb->priority == 1) {
        printf("Putting process pid %d to medium priority queue... \n", pid);
        List_append(medium_priority, pcb);
        return;
    } else {
        printf("Putting process pid %d to medium priority queue... \n", pid);
        List_append(medium_priority, pcb);
        return;
    }
    printf("SUCCESS...\n");
}

void remove_pcb(int pid) {
    List_first(high_priority);
    List_first(medium_priority);
    List_first(low_priority);
    List_first(send_blocker);
    List_first(receive_blocker);
    
    if (List_search(high_priority, compare_pid, &pid) != NULL) {
        printf("Removing process pid %d from high priority queue... \n", pid);
        List_remove(high_priority);
    } else if (List_search(medium_priority, compare_pid, &pid) != NULL) {
        printf("Removing process pid %d from medium priority queue... \n", pid);
         List_remove(medium_priority);
    } else if (List_search(low_priority, compare_pid, &pid) != NULL) {
        printf("Removing process pid %d from low priority queue... \n", pid);
         List_remove(low_priority);
    } else if (List_search(send_blocker, compare_pid, &pid) != NULL) {
        printf("Removing process pid %d from send blocker queue... \n", pid);
         List_remove(send_blocker);
    } else if (List_search(receive_blocker, compare_pid, &pid) != NULL) {
        printf("Removing process pid %d from receive blocker queue... \n", pid);
         List_remove(receive_blocker);
    } else {
        printf("There is no process with pid %d in the system. Cannot kill!!\n", pid);
        printf("FAILURE...\n");
        return;
    }

    printf("Removing process pid %d from the system... \n", pid);
    List_first(list_pcb);
    void* pcb = List_search(list_pcb, compare_pid, &pid);
    List_remove(list_pcb);

    free(pcb);
    printf("SUCCESS...\n");
}
void next_pcb() {
    if (curr_pcb != NULL) {
        curr_pcb->state = READY;
        if (curr_pcb->priority == 0) {
            curr_pcb->priority = 1;
            List_append(medium_priority, curr_pcb);
        } else if (curr_pcb->priority == 1) {
            curr_pcb->priority = 2;
            List_append(low_priority, curr_pcb);
        } else {
            List_append(low_priority, curr_pcb);
        }
    }

    // If there is a high, med or low priority pcb is waiting then move them to the running processor
    if (List_count(high_priority) > 0) {
        List_first(high_priority);
        curr_pcb = List_remove(high_priority);
        curr_pcb->state = RUNNING;
        printf("Current process switch to pid %d\n", curr_pcb->pid);
        return;
    } else if (List_count(medium_priority) > 0) {
        List_first(medium_priority);
        curr_pcb = List_remove(medium_priority);
        curr_pcb->state = RUNNING;
        printf("Current process switch to pid %d\n", curr_pcb->pid);
        return;
    } else if (List_count(low_priority) > 0) {
        List_first(low_priority);
        curr_pcb = List_remove(low_priority);
        curr_pcb->state = RUNNING;
        printf("Current process switch to pid %d\n", curr_pcb->pid);
        return;
    } else {
        curr_pcb = init_pcb;
        printf("Current process switch to init process\n");
        return;
    }

    printf("SUCCESS...\n");
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
    list_sem = List_create();
}

void create_pcb(int priority) {
    pid++;
    PCB* proc = malloc(sizeof(PCB));
    proc->pid = pid;
    proc->priority = priority;
    proc->state = READY;
    proc->proc_message = NULL;

    List_append(list_pcb, proc);
    if (curr_pcb == init_pcb) {
        proc->state = RUNNING;
        curr_pcb = proc;
    } else {
        // Adding to waiting list
        if (priority == 0) {
            List_append(high_priority, proc);
        } else if (priority == 1) {
            List_append(medium_priority, proc);
        } else {
            List_append(low_priority, proc);
        }
    }

    printf("Successfully creating new pcb with pid %d !!\n", pid);
    printf("SUCCESS...\n");
}

int fork_pcb() {
    if (curr_pcb == NULL) {
        printf("There is no process is running currently. Cannot fork!!\n");
        printf("FAILURE...\n");
        return -1;
    } else if (curr_pcb->pid == 0) {
        printf("Trying to fork init processor. Cannot fork!!\n");
        printf("FAILURE...\n");
        return -1;
    }

    pid++;
    PCB* proc = malloc(sizeof(PCB));
    proc->pid = pid;
    proc->priority = curr_pcb->priority;
    proc->state = READY;
    proc->proc_message = curr_pcb->proc_message;

    // Adding to waiting list
    List_append(list_pcb, proc);
    if (proc->priority == 0) {
        List_append(high_priority, proc);
    } else if (proc->priority == 1) {
        List_append(medium_priority, proc);
    } else {
        List_append(low_priority, proc);
    }

    printf("SUCCESS...\n");
    return proc->pid;
}


void kill_pcb(int pid) {
    if (curr_pcb->pid == pid) {
        exit_pcb();
        return;
    }

    if (pid == 0) {
        if (List_count(list_pcb) > 0) {
            printf("Trying to kill init processor while other processes still running. Cannot kill!!\n");
            printf("FAILURE...\n");
            return;
        } else {
            printf("Terminate... See you later UwU.");
            exit(1);
        }
    }

    // Remove from the pcb, priority, send, receive and semaphores
    remove_pcb(pid);
}

void exit_pcb() {
    if (curr_pcb->pid == 0) {
        if (List_count(list_pcb) > 0) {
            printf("Trying to kill init processor while other processes still running. Cannot kill!!\n");
            printf("FAILURE...\n");
            return;
        } else {
            printf("Terminate... See you later UwU.");
            exit(1);
        }
    }

    printf("Removing current process pid %d ...\n", curr_pcb->pid);
    List_first(list_pcb);
    List_search(list_pcb, compare_pid, &curr_pcb->pid);
    List_remove(list_pcb);

    free(curr_pcb);
    curr_pcb = NULL;
    printf("SUCCESS...\n");

    next_pcb();
}

void total_info_pcb() {
    if (List_count(list_pcb) > 0) {
        int count = 0;
        printf("List of process in the system: \n\n");
        List_first(list_pcb);
        while(List_curr(list_pcb) != NULL) {
            count++;
            PCB* pcb = List_curr(list_pcb);
            char* state;
            if (pcb->state == READY) {
                state = "READY";
            } else if (pcb->state == RUNNING) {
                state = "RUNNING";
            } else {
                state = "BLOCKED";
            }
            printf("Process #%d: \n", count);
            printf("    Pid: %d\n", pcb->pid);
            printf("    Priority: %d\n", pcb->priority);
            printf("    State: %s\n", state);
            printf("    proc_message: %s\n", pcb->proc_message);
            printf("\n");
            List_next(list_pcb);
        }
    } else {
        printf("There are no processes running in the system\n\n");
    }

    printf("\n");

    if (List_count(list_sem) > 0) {
        int count = 0;
        printf("List of semaphore in the system: \n\n");
        List_first(list_sem);
        while(List_curr(list_sem) != NULL) {
            count++;
            SEM* sem = List_curr(list_sem);
            printf("Semaphore #%d: \n", count);
            printf("    Sid: %d\n", sem->sid);
            printf("    Value: %d\n", sem->val);
            printf("    Initial Value: %d\n", sem->initVal);
            printf("\n");
            List_next(list_sem);
        }
    } else {
        printf("There are no semaphores in the system\n\n");
    }
}

void quantum_pcb() {
    if (curr_pcb->pid == 0) {
        printf("Currently init processor is running!!. Cannot quantum\n");
        printf("FAILURE...\n");
        return;
    }

    next_pcb();
}

void create_sem(int sid, int initVal) {

    // Searching from semaphore list if the sid is already existed or not
    List_first(list_sem);
    if (List_search(list_sem, compare_sid, &sid) != NULL) {
        printf("The semaphore with sid %d is already existed in the system. Please try with different ID!!\n", sid);
        printf("FAILURE...\n");
        return;
    }

    SEM* sem = malloc(sizeof(sem));
    sem->sid = sid;
    sem->initVal = initVal;
    sem->val = initVal;
    sem->waitList = List_create();

    List_append(list_sem, sem);
    printf("Successfully creating new semaphore with sid %d !!\n", sid);
    printf("SUCCESS...\n");
}

void proc_info(int pid) {
    List_first(list_pcb);
    PCB* pcb = List_search(list_pcb, compare_pid, &pid);

    if (pcb != NULL) {
        char* state;
        if (pcb->state == READY) {
            state = "READY";
        } else if (pcb->state == RUNNING) {
            state = "RUNNING";
        } else {
            state = "BLOCKED";
        }
        printf("Information of process: \n");
        printf("    Pid: %d\n", pcb->pid);
        printf("    Priority: %d\n", pcb->priority);
        printf("    State: %s\n", state);
        printf("    proc_message: %s\n", pcb->proc_message);
        printf("SUCCESS...\n");
        printf("\n");
    } else {
        printf("There are no process with pid %d in the system. Please try again!!\n", pid);
        printf("FAILURE...\n");
    }
}

void send_pcb(int pid, char* msg) {
     if (curr_pcb->pid == 0) {
        printf("Init Process is running. Cannot sending!!\n");
        printf("FAILURE...\n");
        return;
    }

    if (pid == curr_pcb->pid) {
        printf("You are sending message to yourself. Please try again!!\n");
        printf("FAILURE...\n");
        return;
    }

    List_first(list_pcb);
    PCB* receiver_pcb = List_search(list_pcb, compare_pid, &pid);
    if (receiver_pcb == NULL) {
        printf("There is no process with pid %d in the system to send message. Please try again!!\n", pid);
        printf("FAILURE...\n");
        return;
    }

    if (receiver_pcb->proc_message != NULL) {
        printf("The receiver process has not finished reading their message yet. Please try again after they receiving!!\n");
        printf("FAILURE...\n");
        return;
    }

    // Attach message to receiver
    printf("Successfully send message...\n");
    receiver_pcb->proc_message = msg;

    // Blocking sender until it gets reply
    printf("Blocking current process until receiving reply...\n");
    curr_pcb->state = BLOCKED;
    List_append(send_blocker, curr_pcb);

    // Unblock receiver if they are waiting message arrive
    List_first(receive_blocker);
    PCB* receive_blocker_pcb = List_search(receive_blocker, compare_pid, &pid);
    if (receive_blocker_pcb != NULL) {
        List_remove(receive_blocker);
        put_pcb(receive_blocker_pcb);
    }

    // Move to the next waiting process
    curr_pcb = NULL;
    next_pcb();
}

void receive_pcb() {
    if (curr_pcb->pid == 0) {
        printf("Init Process is running. Cannot receiving!!\n");
        printf("FAILURE...\n");
        return;
    }

    // If there is message then read it
    if (curr_pcb->proc_message != NULL) {
        printf("New message received: %s\n\n", curr_pcb->proc_message);
        free(curr_pcb->proc_message);
        curr_pcb->proc_message = NULL;
        return;
    }
    
    // Blocking receiver until there is message arrives
    printf("Blocking current process until receiving message...\n");
    curr_pcb->state = BLOCKED;
    List_append(receive_blocker, curr_pcb);

    // Move to the next waiting process
    curr_pcb = NULL;
    next_pcb();
}