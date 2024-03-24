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

// Initializes the simulation, displays startup message.
void intro () { 
    printf("Welcome to OS simulation\n");
    printf("Below is all the commands available  : \n" ) ;
    printf(
    "|-----------------|-----------------------------------|---------|\n"
    "| Commands        | User Input                        | Example |\n"
    "|-----------------|-----------------------------------|---------|\n"
    "| Create          | C [priority]                      | C 1     |\n"
    "| Fork            | F                                 | F       |\n"
    "| Kill            | K [pid]                           | K 1     |\n"
    "| Exit            | E                                 | E       |\n"
    "| Quantum         | Q                                 | Q       |\n"
    "| Send            | S [pid] [message]                 | S 1 hi  |\n"
    "| Receive         | R                                 | R       |\n"
    "| Reply           | Y [pid] [message]                 | Y 1 hi! |\n"
    "| New Semaphores  | N [semaphore id] [initial value]  | N 2 0   |\n"
    "| Semaphore P     | P [semaphore id]                  | P 2     |\n"
    "| Semaphore V     | V [semaphore id]                  | V 2     |\n"
    "| ProcInfo        | I [pid]                           | I 1     |\n"
    "| Totalinfo       | T                                 | T       |\n"
    "-----------------------------------------------------------------\n"
        );

}

// Compares PCB's PID with a given PID, returns true if they match.
bool compare_pid(void* pcb, void* pid) {
    return ((PCB*)pcb)->pid == *(int*)pid;
}

// Compares semaphore's ID with a given ID, returns true if they match.
bool compare_sid(void* sem, void* sid) {
    return ((SEM*)sem)->sid == *(int*)sid;
}

// Frees memory allocated for a PCB.
void free_proc(void* item) {
    PCB* pcb = (PCB*)item;
    if (pcb != NULL) {
         if (pcb->proc_message != NULL) {
            free(pcb->proc_message);
        }

        free(pcb);
    }
}

// Frees memory allocated for a semaphore.
void free_sem(void* item) {
    SEM* sem = (SEM*)item;
    if (sem != NULL) {
        List_free(sem->waitList, free_proc);
        free(sem);
    }
}

 // Frees all allocated memory for PCBs, semaphores, and process queues, and resets the system state.
void system_free() {
    List_free(high_priority, free_proc);
    List_free(medium_priority, free_proc);
    List_free(low_priority, free_proc);
    List_free(send_blocker, free_proc);
    List_free(receive_blocker, free_proc);
    List_free(list_sem, free_sem);
    List_free(list_pcb, free_proc);

    free_proc(init_pcb);
}

// Prints PCB detail information
void print_pcb(PCB* pcb, int count) {
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
    printf("    block_message: %s\n", pcb->block_message);
    printf("\n");
}

// Adds a PCB to the appropriate queue based on its priority.
void put_pcb(PCB* pcb) {
    if (pcb == NULL) {
        printf("There is no process to put it in waiting queue. Please try again!!\n");
        return;
    }

    // If Init process is running then run the pcb
    if (curr_pcb->pid == 0) {
        pcb->state = RUNNING;
        curr_pcb = pcb;
        printf("Current process switch to pid %d\n", curr_pcb->pid);
        return;
    }

    pcb->state = READY;
    if (pcb->priority == 0) {
        printf("Putting process pid %d to high priority queue... \n", pcb->pid);
        List_append(high_priority, pcb);
        return;
    } else if (pcb->priority == 1) {
        printf("Putting process pid %d to medium priority queue... \n", pcb->pid);
        List_append(medium_priority, pcb);
        return;
    } else {
        printf("Putting process pid %d to low priority queue... \n", pcb->pid);
        List_append(low_priority, pcb);
        return;
    }
    printf("SUCCESS...\n");
}

// Removes a PCB from its queue based on PID.
void remove_pcb(int pid) {
    List_first(high_priority);
    List_first(medium_priority);
    List_first(low_priority);
    List_first(send_blocker);
    List_first(receive_blocker);

    bool inSem = false;
    
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
        List_first(list_sem);
        while (List_curr(list_sem) != NULL) {
            SEM* sem = List_curr(list_sem);
            List_first(sem->waitList);
            if (List_search(sem->waitList, compare_pid, &pid) != NULL) {
                printf("Removing process pid %d from semaphore sid %d blocker queue...\n", pid, sem->sid);
                List_remove(sem->waitList);
                inSem = true;
                break;
            } else {
                List_next(list_sem);
            }
        }

        if (inSem == false) {
            printf("There is no process with pid %d in the system. Cannot kill!!\n", pid);
            printf("FAILURE...\n");
            return;
        }
    }

    printf("Removing process pid %d from the system... \n", pid);
    List_first(list_pcb);
    void* pcb = List_search(list_pcb, compare_pid, &pid);
    List_remove(list_pcb);

    free(pcb);
    printf("SUCCESS...\n");
}

// Switches context to the next PCB in the ready queue.
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
    } else if (List_count(medium_priority) > 0) {
        List_first(medium_priority);
        curr_pcb = List_remove(medium_priority);
        curr_pcb->state = RUNNING;
        printf("Current process switch to pid %d\n", curr_pcb->pid);
    } else if (List_count(low_priority) > 0) {
        List_first(low_priority);
        curr_pcb = List_remove(low_priority);
        curr_pcb->state = RUNNING;
        printf("Current process switch to pid %d\n", curr_pcb->pid);
    } else {
        curr_pcb = init_pcb;
        printf("Current process switch to init process\n");
        return;
    }

    if (curr_pcb->block_message != NULL) {
        printf("### TING TING receive message ###\n");
        printf("Message: %s\n\n", curr_pcb->block_message);
        free(curr_pcb->block_message);
        curr_pcb->block_message = NULL;
    }
}

// Creates the "init" PCB at simulation start.
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

// Creates a new PCB with the specified priority.
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

// Duplicates the currently running PCB.
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
    proc->proc_message = NULL;
    proc->block_message = NULL;

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

// Terminates a PCB based on PID.
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
            printf("Terminate... See you later UwU.\n");
            system_free();
            exit(1);
        }
    }

    // Remove from the pcb, priority, send, receive and semaphores
    remove_pcb(pid);
}


// Terminates the currently running PCB.
void exit_pcb() {
    if (curr_pcb->pid == 0) {
        if (List_count(list_pcb) > 0) {
            printf("Trying to kill init processor while other processes still running. Cannot kill!!\n"); printf("Current process switch to pid %d\n", curr_pcb->pid);
            printf("FAILURE...\n");
            return;
        } else {
            printf("Terminate... See you later UwU.\n");
            system_free();
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

// Displays detail information about queue and pcb
void total_info_pcb() {
    int count = 0;
    if (List_count(list_pcb) == 0) {
        printf("There are no processes running in the system\n\n");
        return;
    }

    // Print current process running
    if (curr_pcb == NULL || curr_pcb->pid == 0) {
        printf("### There is no process that running in the system (Init process is running) ###\n\n");
    } else {
        printf("### Information's process currently running in the system ###\n\n");
        count++;
        print_pcb(curr_pcb, count);
    }

    // Print process that waiting in high priority queue
    if (List_count(high_priority) == 0) {
        printf("### There are no processes that waiting in high priority queue ###\n\n");
    } else {
        printf("### List of proccesses that waiting in high priority queue ###\n\n");
        List_first(high_priority);
        while(List_curr(high_priority) != NULL) {
            count++;
            PCB* pcb = List_curr(high_priority);
            print_pcb(pcb, count);
            List_next(high_priority);
        }
    }

    // Print process that waiting in medium priority queue
    if (List_count(medium_priority) == 0) {
        printf("### There are no processes that waiting in medium priority queue ###\n\n");
    } else {
        printf("### List of proccesses that waiting in medium priority queue ###\n\n");
        List_first(medium_priority);
        while(List_curr(medium_priority) != NULL) {
            count++;
            PCB* pcb = List_curr(medium_priority);
            print_pcb(pcb, count);
            List_next(medium_priority);
        }
    }

    // Print process that waiting in low priority queue
    if (List_count(low_priority) == 0) {
        printf("### There are no processes that waiting in low priority queue ###\n\n");
    } else {
        printf("### List of proccesses that waiting in low priority queue ###\n\n");
        List_first(low_priority);
        while(List_curr(low_priority) != NULL) {
            count++;
            PCB* pcb = List_curr(low_priority);
            print_pcb(pcb, count);
            List_next(low_priority);
        }
    }

    // Print process that blocking by send operation
    if (List_count(send_blocker) == 0) {
        printf("### There are no processes that blocking by send operation ###\n\n");
    } else {
        printf("### List of proccesses that blocking by send operation ###\n\n");
        List_first(send_blocker);
        while(List_curr(send_blocker) != NULL) {
            count++;
            PCB* pcb = List_curr(send_blocker);
            print_pcb(pcb, count);
            List_next(send_blocker);
        }
    }

    // Print process that blocking by receive operation
    if (List_count(receive_blocker) == 0) {
        printf("### There are no processes that blocking by receive operation ###\n\n");
    } else {
        printf("### List of proccesses that blocking by receive operation ###\n\n");
        List_first(receive_blocker);
        while(List_curr(receive_blocker) != NULL) {
            count++;
            PCB* pcb = List_curr(receive_blocker);
            print_pcb(pcb, count);
            List_next(receive_blocker);
        }
    }

    if (List_count(list_sem) > 0) {
        List_first(list_sem);
        while (List_curr(list_sem) != NULL) {
            SEM* sem = List_curr(list_sem);
            if (List_count(sem->waitList) == 0) {
                printf("### There are no processes that blocking by semaphore sid %d ###\n\n", sem->sid);
            } else {
                printf("### List of proccesses that blocking by semaphore sid %d ###\n\n", sem->sid);
                List_first(sem->waitList);
                while(List_curr(sem->waitList) != NULL) {
                    count++;
                    PCB* pcb = List_curr(sem->waitList);
                    print_pcb(pcb, count);
                    List_next(sem->waitList);
                }
            }
            List_next(list_sem);
        }
    }
}

// Handles the end of a time quantum for the currently running PCB and allow other process to run
void quantum_pcb() {
    if (curr_pcb->pid == 0) {
        printf("Currently init processor is running!!. Cannot quantum\n");
        printf("FAILURE...\n");
        return;
    }

    next_pcb();
}

// create a semaphore with a given ID and initial value.
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

// Displays information for a specific PCB by PID.
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

// Sends a message from the current running PCB to another PCB.
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

    // Blocking sender until it gets reply
    printf("Blocking current process until receiving reply...\n");
    curr_pcb->state = BLOCKED;
    List_append(send_blocker, curr_pcb);

    // Unblock receiver if they are waiting message arrive
    List_first(receive_blocker);
    PCB* receive_blocker_pcb = List_search(receive_blocker, compare_pid, &pid);
    if (receive_blocker_pcb != NULL) {
        List_remove(receive_blocker);
        receive_blocker_pcb->block_message = msg;
        put_pcb(receive_blocker_pcb);
    } else {
        // Attach message to receiver
        printf("Successfully send message...\n");
        receiver_pcb->proc_message = msg;
    }

    // Move to the next waiting process
    curr_pcb = NULL;
    next_pcb();
}

// Sets the current running PCB to receive a message and display the message
void receive_pcb() {
    if (curr_pcb->pid == 0) {
        printf("Init Process is running. Cannot receiving!!\n");
        printf("FAILURE...\n");
        return;
    }
    
    if (curr_pcb->proc_message != NULL) {
        printf("Message received: %s\n\n", curr_pcb->proc_message);
        free(curr_pcb->proc_message);
        curr_pcb->proc_message = NULL;
        return;
    } else {
         // Blocking receiver until there is message arrives
        printf("Blocking current process until new message arrives to this process...\n");
        curr_pcb->state = BLOCKED;
        List_append(receive_blocker, curr_pcb);

        // Move to the next waiting process
        curr_pcb = NULL;
        next_pcb();
    }
}

// Sends a reply from the current PCB to another PCB.
void reply_pcb(int pid, char* msg) {
    if (curr_pcb->pid == 0) {
        printf("Init Process is running. Cannot replying!!\n");
        printf("FAILURE...\n");
        return;
    }

    if (pid == curr_pcb->pid) {
        printf("You are replying message to yourself. Please try again!!\n");
        printf("FAILURE...\n");
        return;
    }

    List_first(list_pcb);
    PCB* receiver_pcb = List_search(list_pcb, compare_pid, &pid);
    if (receiver_pcb == NULL) {
        printf("There is no process with pid %d in the system to reply message. Please try again!!\n", pid);
        printf("FAILURE...\n");
        return;
    }

    // Unblock sender if they are waiting reply arrive, otherwise send error if the pid not waiting a reply
    List_first(send_blocker);
    PCB* send_blocker_pcb = List_search(send_blocker, compare_pid, &pid);
    if (send_blocker_pcb != NULL) {
        printf("Reply successfully to process pid %d!!\n", send_blocker_pcb->pid);
        List_remove(send_blocker);
        send_blocker_pcb->block_message = msg;
        put_pcb(send_blocker_pcb);
    } else {
        printf("There are no sender with pid %d is waiting for the reply. Please try again!!\n", pid);
        printf("FAILURE...\n");
        return;
    }
}

// Performs the P operation on a semaphore for the current running PCB.
void p_sem(int sid) {
    if (curr_pcb->pid == 0) {
        printf("Init Process is running. Cannot be blocked!!\n");
        printf("FAILURE...\n");
        return;
    }
    // Searching semaphore with sid
    List_first(list_sem);
    SEM* sem = List_search(list_sem, compare_sid, &sid);
    if (sem == NULL) {
        printf("There are no semaphore with sid. Please try again!!\n");
        printf("FAILURE...\n");
        return;
    }

    printf("Blocking currently process by semaphore sid %d...\n", sid);
    curr_pcb->state = BLOCKED;
    List_append(sem->waitList, curr_pcb);
    sem->val--;

    // Move to the next waiting process
    curr_pcb = NULL;
    next_pcb();
}

// Performs the V operation on a semaphore for the current running PCB
void v_sem(int sid) {
    // Searching semaphore with sid
    List_first(list_sem);
    SEM* sem = List_search(list_sem, compare_sid, &sid);
    if (sem == NULL) {
        printf("There are no semaphore with sid. Please try again!!\n");
        printf("FAILURE...\n");
        return;
    }

    if (sem->val < sem->initVal) {
        sem->val++;
        List_first(sem->waitList);
        PCB* unblocked_pcb = List_remove(sem->waitList);
        printf("Unblocking process pid %d from semaphore sid %d!!\n", unblocked_pcb->pid, sem->sid);
        put_pcb(unblocked_pcb);
    } else {
        printf("There are no processes that blocking by this semaphore. Please try again!!");
        printf("FAILURE...\n");
        return;
    }
}