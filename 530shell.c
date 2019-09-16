// This program was written by Victor Murta
// It is a simple shell program that takes in command names from stdin
// and executes them
// I pledge that I have neither given nor received unauthorized aid on this assignment
#include "stdio.h"
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#define MAX_ARGS 100
#define MAX_LENGTH 1000

int isParent = 1;
int numChildProcesses = 0;

void ctrlCSignalHandler(int signal_number){
    if (isParent == 0 || numChildProcesses == 0) {
        printf("\nExiting Process\n");
        fflush(stdout);
        exit(0);
    }
    fflush(stdout);
}

// void childHandler(int signal_number){
//     wait(2);
//     numChildProcesses--;
// }

void parseArgs(char *command, char **argv){
    char *token = strtok(command, " \n");
    *argv++ = token;
    while (token != NULL){
        token = strtok(NULL, " \n");
        *argv++ = token;
    }
}

int main (int argc, char* argv) {
    char *line = (char*) malloc(sizeof(char) * MAX_LENGTH);
    signal(SIGINT, ctrlCSignalHandler); 
    // print initial % signifying ready for input
    printf ("%% ");

    // keep iterating until end of file is reached
    while(fgets(line, MAX_LENGTH, stdin)) {

        // case where buffer overflows, don't run anything
        if (line[strlen(line) -1] != '\n') {
            printf("Character limit was exceeded, this command was not run\n");

            // skip to the end of this whole ass line of code
            char c;
            while ((c = fgetc(stdin)) != '\n' && c != EOF);
        }
        // valid input
        else {

            // child process
            isParent = fork();
            if (!isParent){
                
                char *argv[MAX_ARGS];
                parseArgs(line, argv);

                int ok = execvp(argv[0], argv);
                if (ok < 0) {
                fprintf(stderr, "Error executing command: %s\n", strerror( errno ));                   
                }
                exit(0);
            }
            // parent process 
            else if (isParent > 0){
                numChildProcesses++;
                wait(NULL);
                numChildProcesses--;
                // signal(SIGCHLD, childHandler);
            // parent process handles the error
            } else {
                fprintf(stderr, "Error forking child: %s\n", strerror( errno ));
            }
        }
        printf ("%% ");
    }
}