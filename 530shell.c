// This program was written by Victor Murta
// It is a simple shell program that takes in command names from stdin
// and executes them
// I pledge that I have neither given nor received unauthorized aid on this assignment
#include "stdio.h"
#include <string.h>
#define MAX 100

int main (int argc, char* argv) {
    char line[MAX];

    // print initial % signifying ready for input
    printf ("%% ");

    // keep iterating until end of file is reached
    while(fgets(line, MAX, stdin)) {

        // case where buffer overflows, don't run anything
        if (line[strlen(line) -1] != '\n') {
            printf("Character limit was exceeded, this command was not run\n");

            // skip to the end of this whole ass line of code
            char c;
            while ((c = fgetc(stdin)) != '\n' && c != EOF);
        }
        // valid input
        else {
            printf("%s", line);
        }
        printf ("%% ");
    }
}