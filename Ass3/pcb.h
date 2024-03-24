#ifndef _PCB_H_
#define _PCB_H_

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
    char* block_message;
};

typedef struct SEM_s SEM;
struct SEM_s {
    int sid;
    int val;
    int initVal;
    List* waitList;
};

void intro () ; // Initializes the simulation, displays startup message.
bool compare_pid(void* pcb, void* pid); // Compares PCB's PID with a given PID, returns true if they match
bool compare_sid(void* sem, void* sid); // Compares semaphore's ID with a given ID, returns true if they match.
void free_proc(void* item); // Frees memory allocated for a PCB.
void free_sem(void* item); // Frees memory allocated for a semaphore.
void system_free();  // Frees all allocated memory for PCBs, semaphores, and process queues, and resets the system state.
void print_pcb(PCB* pcb, int count); // Prints PCB detail information
void put_pcb(PCB* pcb); // Adds a PCB to the appropriate queue based on its priority
void remove_pcb(int pid); // Removes a PCB from its queue based on PID.
void next_pcb(); // Switches context to the next PCB in the ready queue
void create_pcb_init(); // Creates the "init" PCB at simulation start
void create_pcb(int priority); // Creates a new PCB with the specified priority.
int fork_pcb(); // Duplicates the currently running PCB.
void quantum_pcb(); // Handles the end of a time quantum for the currently running PCB and allow other process to run
void kill_pcb(int pid); // Terminates a PCB based on PID.
void exit_pcb(); // Terminates the currently running PCB.
void total_info_pcb(); // Displays detail information about queue and pcb
void create_sem(int sid, int initVal); // create a semaphore with a given ID and initial value.
void proc_info(int pid);// Displays information for a specific PCB by PID.
void send_pcb(int pid, char* msg); // Sends a message from the current running PCB to another PCB.
void receive_pcb(); // Sets the current running PCB to receive a message and display the message
void reply_pcb(int pid, char* msg); // Sends a reply from the current PCB to another PCB.
void p_sem(int sid); // Performs the P operation on a semaphore for the current running PCB.
void v_sem(int sid); // Performs the V operation on a semaphore for the current running PCB

#endif

