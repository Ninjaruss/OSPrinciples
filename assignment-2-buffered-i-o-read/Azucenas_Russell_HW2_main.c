/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* GitHub UserID: Ninjaruss
* Project: Assignment 2 – Buffered I/O
*
* File: Azucenas_Russell_HW2_main.c
*
* Description:
* Tests b_io.c; reads DecOfInd.txt
**************************************************************/
#include <stdio.h>
#include <b_io.h>
#include "stdlib.h" // malloc() and free()

#define BUFFERCNT 80

int main(int argc, char *argv[])
{
   	int readCount;
	char * buffer;
	b_io_fd fd;

	if (argc != 2)
	{
		printf ("Proper usage is: command filename\n");
		return -1;
	}
		
	buffer = malloc(BUFFERCNT + 2);	//allocate a little extra space, need at least one for the null.

	/***  ToDo:  Check if malloc succeeds  ***/
	if (buffer == NULL)
	{
		printf("malloc failed.");
		return -1;
	}

	fd = b_open (argv[1]);

	/***  ToDo:  Check if b_open succeeds  ***/

	do
	{
		readCount = b_read (fd, buffer, BUFFERCNT);
		buffer[readCount] = '\0';
		printf ("%s\n", buffer);
	} while (readCount > 0);

	/***  TODO add b_close and free anything that needs freeing  ***/
	b_close(fd);
	free(buffer);
	buffer = NULL;

	return 0;
}