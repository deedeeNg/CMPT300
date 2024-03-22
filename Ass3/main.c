#include "pcb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    create_pcb_init();

    while (1) {
        printf("> ");
		char input[40];
		memset(&input, 0, 40);
		fgets(input, 40, stdin);
		char text[40]; // Text input, used for pids and messages
        printf("============================================================\n\n");

        switch (input[0]) {
            case 'C':
                if (input[1] != ' ' || input[3] != '\n') {
                    printf("Create format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }
                int priority = input[2] - '0';
                if (priority > 2 || priority < 0) {
                    printf("Priority should be between 0 and 2, please try again!!\n");
                    printf("FAILURE...\n");
                    break;   
                }

                create_pcb(priority);
                break;
            
            case 'F':
                if (input[1] != '\n') {
                    printf("Fork format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }
                int pid = fork_pcb();
                printf("Forking current running process successfully, new process pid is %d\n", pid);
                break;
            
            case 'T':
                if (input[1] != '\n') {
                    printf("TotalInfo format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }
                total_info_pcb();
                break;

            case 'Q':
                if (input[1] != '\n') {
                    printf("Quantum format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                }
                quantum_pcb();
                break;

            case 'E':
                if (input[1] != '\n') {
                    printf("Exit format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                }
                exit_pcb();
                break;

            case 'K':
                if (input[1] != ' ') {
                    printf("Kill format is wrong please try again!!\n");
                    printf("FAILURE...\n");
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
                kill_pcb(kill_pid);
                break;

            case 'N':
                if (input[1] != ' ') {
                    printf("Create new semaphore is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                char * input_text = input + 2;
                index = 0;
                while (*input_text != ' ' && *input_text != '\n') {
                    text[index] = *input_text;
                    index++;
                    input_text++;
                }

                int sid = atoi(text);
                if (sid < 0 || sid > 4) {
                    printf("ID of semaphore should be in range of 0 and 4. Please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                memset(&text, 0, 40);
                index = 0;
                input_text++;
                while (*input_text != '\n') {
                    text[index] = *input_text;
                    index++;
                    input_text++;
                }

                int initVal = atoi(text);
                if (initVal < 0) {
                    printf("initial value of semaphore should be larger than 0. Please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                create_sem(sid, initVal);
                break;

            case 'I':
                if (input[1] != ' ') {
                    printf("Procedure process information format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                index = 0;
                pid_text = input + 2;
                while (*pid_text != '\n') {
                    text[index] = *pid_text;
                    index++;
                    pid_text++;
                }

                pid = atoi(text);
                proc_info(pid);
                break;

            case 'S':
                if (input[1] != ' ') {
                    printf("Sending format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                index = 0;
                input_text = input + 2;
                while (*input_text != ' ' && *input_text != '\n') {
                    text[index] = *input_text;
                    index++;
                    input_text++;
                }

                pid = atoi(text);

                memset(&text, 0, 40);
                index = 0;
                input_text++;
                while (*input_text != '\n') {
                    text[index] = *input_text;
                    index++;
                    input_text++;
                }

                text[++index] = '\0';

                char* msg = malloc(40 * sizeof(char));
                strcpy(msg, text);

                send_pcb(pid, msg);
                break;

            case 'R':
                if (input[1] != '\n') {
                    printf("Receive format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                receive_pcb();
                break;

            case 'Y':
                if (input[1] != ' ') {
                    printf("replying format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                index = 0;
                input_text = input + 2;
                while (*input_text != ' ' && *input_text != '\n') {
                    text[index] = *input_text;
                    index++;
                    input_text++;
                }

                pid = atoi(text);

                memset(&text, 0, 40);
                index = 0;
                input_text++;
                while (*input_text != '\n') {
                    text[index] = *input_text;
                    index++;
                    input_text++;
                }

                text[++index] = '\0';

                msg = malloc(40 * sizeof(char));
                strcpy(msg, text);

                reply_pcb(pid, msg);
                break;

            case 'P':
                if (input[1] != ' ') {
                    printf("Semaphore P format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                pid_text = input + 2;
                index = 0;
                while (*pid_text != '\n') {
                    text[index] = *pid_text;
                    index++;
                    pid_text++;
                }

                text[++index] = '\0';
                sid = atoi(text);
                p_sem(sid);
                break;

            case 'V':
                if (input[1] != ' ') {
                    printf("Semaphore V format is wrong please try again!!\n");
                    printf("FAILURE...\n");
                    break;
                }

                pid_text = input + 2;
                index = 0;
                while (*pid_text != '\n') {
                    text[index] = *pid_text;
                    index++;
                    pid_text++;
                }

                text[++index] = '\0';
                sid = atoi(text);
                v_sem(sid);
                break;

            default:
                printf("Cannot recognize input. Please try again!!\n");
                break;
        }
    }

    return 0;
}