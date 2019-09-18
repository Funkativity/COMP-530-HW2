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
#include <sys/stat.h>
#define MAX_ARGS 100
#define MAX_LENGTH 1000

int isParent = 1;
int numChildProcesses = 0;

void ctrlCSignalHandler(int signal_number){
    if (isParent == 0 || numChildProcesses == 0) {
        exit(0);
    }
}

// void childHandler(int signal_number){
//     wait(2);
//     numChildProcesses--;
// }

// string parser, splits up a string (command) ending with \n based off spaces
// writes them into array of strings argv
void parseArgs(char *command, char **argv){
    char *token = strtok(command, " \n\t");
    *argv++ = token;
    while (token != NULL){
        token = strtok(NULL, " \n\t");
        *argv++ = token;
    }
}

int main (int argc, char* argv) {
    char *line = (char*) malloc(sizeof(char) * MAX_LENGTH);
    signal(SIGINT, ctrlCSignalHandler);
    // print initial % signifying ready for input
    printf ("%% ");
    fflush(stdout);
    // keep iterating until end of file is reached
    while(fgets(line, MAX_LENGTH, stdin)) {
        // case where buffer overflows, don't run anything
        if (line[strlen(line) -1] != '\n') {
            fprintf(stderr, "\nCharacter limit was exceeded, this command was not run\n");

            // skip to the end of this whole ass line of code
            char c;
            while ((c = fgetc(stdin)) != '\n' && c != EOF);
        }

        // valid input
        else {

            isParent = fork();

            // child process
            if (!isParent){

                char *argv[MAX_ARGS];
                parseArgs(line, argv);

                // check if the command actually exists
                struct stat file_info;
                if (stat(argv[0], &file_info) < 0){
                    // if we were unable to find the command and a file path was given
                    if (strchr(argv[0], '/') != NULL){
                        fprintf(stderr, "\nUnable to find command\n");
                        exit(0);
                    }

                    // search the path for it;
                    char *paths = getenv("PATH");
                    char *potential_path = strtok(paths, ":");
                    char potential_full_path[MAX_LENGTH];
                    // concatenate the path and the command name
                    snprintf(potential_full_path, sizeof(potential_full_path), "%s%s", potential_path, argv[0]);

                    // test every path
                    while (stat(potential_full_path, &file_info) < 0 && (potential_path = strtok(NULL, ":")) != NULL){
                        snprintf(potential_full_path, sizeof(potential_full_path), "%s%c%s", potential_path, '/', argv[0]);
                    }

                    //all paths failed
                    if (potential_path == NULL){
                        fprintf(stderr, "\nUnable to find command\n");
                        exit(0);
                    // foud it!
                    } else {
                        argv[0] = potential_full_path;
                    }
                }

                int ok = execv(argv[0], argv);
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
            }

            // parent process handles the error
            else {
                fprintf(stderr, "Error forking child: %s\n", strerror( errno ));
            }
        }
        printf ("%% ");
    }
}