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
};

typedef struct SEM_s SEM;
struct SEM_s {
    int sid;
    int val;
    int initVal;
    List* waitList;
};

bool compare_pid(void* pcb, void* pid);
void remove_pcb(int pid);
void next_pcb();
void create_pcb_init();
void create_pcb(int priority);
int fork_pcb();
void quantum_pcb();
void kill_pcb(int pid);
void exit_pcb();
void total_info_pcb();
void create_sem(int sid, int initVal);

#endif

