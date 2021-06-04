/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* GitHub UserID: Ninjaruss
* Project: Assignment 3 – Simple Shell
*
* File: Azucenas_Russell_HW3_main.c
*
* Description:
* Acts as a simple shell when executed. Receives args then executes commands via separate child process.
**************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFERCNT 512
#define MAX_ARGS 100

int main (int argc, char * argv[])
{
    char inStr[BUFFERCNT];
    char programPath[50];
    char * path = "/bin/";
    char * arguments[MAX_ARGS];

    char * defaultPrefix = ">";
    char * prefix;
    
    printf("\n[-Entered Simple Shell-]\n");
    while (1){
        if (argc > 1)
        {
            prefix = argv[1];
        }

        if (prefix != NULL)
        {
            printf("%s", prefix);
        }
        else
        {
            printf("%s", defaultPrefix);
        }
        
        fgets(inStr, BUFFERCNT, stdin);
        fflush(stdin);
        // printf("The Input string is %d bytes and is: %s \n", strlen(inStr), inStr);

        if (inStr[strlen(inStr)-1] == '\n')
        {
            inStr[strlen(inStr)-1] = '\0';
        }
            
        // Exit shell command
        if (strcmp(inStr, "exit") == 0)
        {
            printf("[-Exited Simple Shell-]\n");
            exit(0);
        }

        // Catch empty line
        if (strcmp(inStr, "") == 0)
        {
            printf("Error: No input found. \n");
            continue;
        }

        char * currentToken;
        currentToken = strtok(inStr, " ");

        // Split up the string into tokens and insert them into argv
        int i = 0;
        while (currentToken)
        {
            // Set arg at index to current token
            arguments[i] = currentToken;
            // Go to next token
            currentToken = strtok(NULL, " ");
            i++;
        }
        // Need to set last arg to null for execvp
        arguments[i] = NULL;


        // String together the path to the file and its arguments
        strcpy(programPath, path);
        strcat(programPath, arguments[0]);


        // Let child handle the execution and wait for it to finish
        int pid = fork();
        if (pid < 0){
            printf("Error: Forking child failed. \n");
            exit(1);
        }

        if (pid == 0)
        {
            if (execvp(programPath, arguments) == -1)
            {
                printf("Error: Command execution failed. \n");
                exit(1);
            }
        }
        else
        {
            // Print out the child id and exit code
            int status;
            int cid = waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                printf("[pid:%d] Child exited with %d. \n", cid, WEXITSTATUS(status));
            }
        }
    }
}