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

bool compare_pid(void* pcb, void* pid);
void remove_pcb(int pid);
void next_pcb();
void create_pcb_init();
void create_pcb(int priority);
int fork_pcb();
void kill_pcb(int pid);
void total_info_pcb();

#endif

