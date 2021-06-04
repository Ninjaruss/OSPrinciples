/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* Github Name: Ninjaruss
* Project: Assignment 1 – Command Line Arguments
*
* File: Azucenas_Russell_HW1_main.c
*
* Description:
* Displays the number of passed arguments and lists each one within the console.
**************************************************************/
#include <stdio.h>

int main(int argc, char *argv[]){
    // Check for null args
    if (argc <= 0)
    {
        printf("No arguments found.\n");
    } 
    else
    {
        for (int i = 0; i < argc; i++)
        {
            // %d takes 1st arg and prints as int
            // %s takes 2nd arg and prints as string
            printf("Argument #%d: %s\n", i, argv[i]);
        }
    }
}