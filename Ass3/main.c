#include "pcb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    create_pcb_init();

    while (1) {
        printf("> ");
		char input[50];
		memset(&input, 0, 50);
		fgets(input, 50, stdin);
		char text[50]; // Text input, used for pids and messages

        switch (input[0]) {
            case 'C':
                if (input[1] != ' ' || input[3] != '\n') {
                    printf("Create format is wrong please try again!!\n");
                    break;
                }
                int priority = input[2] - '0';
                if (priority > 2 || priority < 0) {
                    printf("Priority should be between 0 and 2, please try again!!\n");
                    break;
                }

                create_pcb(priority);
                break;
            
            case 'F':
                if (input[1] != '\n') {
                    printf("Fork format is wrong please try again!!\n");
                    break;
                }
                int pid = fork_pcb();
                printf("Forking current running process successfully, new process pid is %d\n", pid);
                break;
            
            case 'T':
                if (input[1] != '\n') {
                    printf("TotalInfo format is wrong please try again!!\n");
                    break;
                }
                total_info_pcb();
                break;

            case 'Q':
                if (input[1] != '\n') {
                    printf("Quantum format is wrong please try again!!\n");
                }
                quantum_pcb();
                break;

            case 'E':
                if (input[1] != '\n') {
                    printf("Exit format is wrong please try again!!\n");
                }
                exit_pcb();
                break;

            case 'K':
                if (input[1] != ' ') {
                    printf("Kill format is wrong please try again!!\n");
                    break;
                }

                char* pid_text = input + 2;
                int index = 0;
                while (*pid_text != '\n') {
                    text[index] = *pid_text;
                    index++;
                    pid_text++;
                }

                text[++index] = '\0';
                int kill_pid = atoi(text);
                printf("%d\n", kill_pid);
                kill_pcb(kill_pid);
                break;

            default:
                printf("Cannot recognize input. Please try again!!\n");
                break;
        }
    }

    return 0;
}