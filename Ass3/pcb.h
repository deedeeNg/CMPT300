#include "list.h"

enum States {
    READY
    RUNNING
    BLOCKED
}

typedef struct pcb {
    int pid;
    int priority;
    States state;
    char* proc_message;
}

