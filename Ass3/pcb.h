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

void intro () ; 
bool compare_pid(void* pcb, void* pid);
bool compare_sid(void* sem, void* sid);
void free_proc(void* item);
void free_sem(void* item);
void system_free();
void print_pcb(PCB* pcb, int count);
void put_pcb(PCB* pcb);
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
void proc_info(int pid);
void send_pcb(int pid, char* msg);
void receive_pcb();
void reply_pcb(int pid, char* msg);
void p_sem(int sid);
void v_sem(int sid);

#endif

