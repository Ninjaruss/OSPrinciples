/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* Project: Assignment 6 – Device Driver
*
* File: test_driver.c
*
* Description: 
*   Tests the char_driver module by asking for a string. 
*   Writes the entered string to the kernel module then reads it back.
*
**************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 256 

int main()
{
    int ret, fd;
    
    // Allocate space for the buffer
    char * charBuffer = (char*) malloc(32);

    printf("Starting device test code example...\n");
    fd = open("/dev/char_driver", O_RDWR);             // Open the device with read/write access
    if (fd < 0){
        perror("Failed to open the device...");
        return errno;
    }

    // Ask for a string to read into the kernel module
    printf("Type in a short string to send to the kernel module:\n");
    scanf("%[^\n]%*c", charBuffer);                // Read in a string (with spaces)

    printf("Writing message to the device [%s].\n", charBuffer);
    ret = write(fd, charBuffer, strlen(charBuffer)); // Send the string to the LKM
    if (ret < 0){
        perror("Failed to write the message to the device.");
        return errno;
    }

    printf("Press ENTER to read back from the device...\n");
    getchar();

    // Ask for how many bytes to read from entered buffer
    int bytesToRead;
    printf("Enter how many bytes to read:\n");
    scanf("%d", &bytesToRead);

    // Adjust charBuffer to match number of bytes desired
    charBuffer = (char *) realloc(charBuffer, bytesToRead);

    // Read the kernel buffer to test buffer
    printf("Reading from the device...\n");
    ret = read(fd, charBuffer, bytesToRead);
    if (ret < 0)
    {
        perror("Failed to read the message from the device.");
        return errno;
    }
	printf("Data read from the device: [%s]\n", charBuffer);

    printf("End of the program\n");
    return 0;
}