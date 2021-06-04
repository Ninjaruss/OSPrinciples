/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* GitHub UserID: Ninjaruss
* Project: Assignment 2 – Buffered I/O
*
* File: b_io.c
*
* Description:
* Creates a buffer open file table (b_OFT) to handle buffering
**************************************************************/

#include "b_io.h"
#include "stdio.h" // printf()
#include "unistd.h" // read()
#include "string.h" // memcpy()
#include "stdlib.h" // malloc()
#include "fcntl.h" // open()

#define B_CHUNK_SIZE 512
#define MAX_OPEN_FILES 20

// b_OFT = buffer open file table
typedef struct b_OFT
{
	int linuxfd; // Identifier for availability, MY INDEX
	char * buffer;
	int bfd; // Buffer's file descriptor, created from open()
	int offset;
	int spaceInBuffer;
	int empty;
} b_OFT;

int initialized = 0;
b_OFT oft[MAX_OPEN_FILES];


void oft_init ()
{
	initialized = 1;
	for (int i = 0; i < MAX_OPEN_FILES; i++)
	{
		oft[i].linuxfd = -1;
		oft[i].buffer = NULL;
		oft[i].bfd = -1;
		oft[i].offset = 0;
		oft[i].spaceInBuffer = 0;
		oft[i].empty = 1;
	}
}

// Gets index of first available space
int b_getIndex(){
	for (int i = 0; i < MAX_OPEN_FILES; i++)
	{
		if (oft[i].linuxfd == -1){
			return i;
		}
	}
	// Return no more space
	return -1;
}

b_io_fd b_open (char * filename)
	{
		if (initialized == 0) oft_init();

		// Check for available spot and return index if available
		int index = b_getIndex();
		if (index < 0){
			return -1;
		}

		// Allocate space for the buffer
		oft[index].buffer = malloc(B_CHUNK_SIZE);
		if (oft[index].buffer == NULL)
		{
			printf("malloc failed.");
			return -1;
		}

		oft[index].linuxfd = index;
		oft[index].bfd = open(filename, O_RDWR);
		oft[index].empty = 0;
		return oft[index].linuxfd;
	}
	
int b_read (b_io_fd fd, char * buffer, int count)
	{
		int transferred = 0;
		int leftover = 0;

		// Need to read if no more space in buffer
		if (oft[fd].spaceInBuffer < count)
		{
			// Check that the file is not empty
			if (oft[fd].empty == 0)
			{
				// Copy the leftover buffer into received buffer
				leftover = count - oft[fd].spaceInBuffer;
				memcpy(buffer, oft[fd].buffer + oft[fd].offset, oft[fd].spaceInBuffer);
				oft[fd].offset = oft[fd].spaceInBuffer;
				transferred += oft[fd].spaceInBuffer;

				// Read the next chunk into buffer
				oft[fd].spaceInBuffer = read(oft[fd].bfd, oft[fd].buffer, B_CHUNK_SIZE);
				if (oft[fd].spaceInBuffer == 0)
				{
					oft[fd].empty = 1;
				}
				// Clear the last few bytes from the current buffer
				else if (oft[fd].spaceInBuffer < B_CHUNK_SIZE)
				{
					int leftover2 = B_CHUNK_SIZE - oft[fd].spaceInBuffer;
					memset(oft[fd].buffer + oft[fd].spaceInBuffer, 0, leftover2 * sizeof(oft[fd].buffer));
				}
			}
			else
			{;
				return 0;
			}
		}

		// Checks if I allocated new space for read
		if (leftover > 0)
		{
			// Handles very last request where spaceInBuffer becomes negative; very hacky solution
			if (oft[fd].spaceInBuffer - leftover < 0)
			{
				return transferred;
			}
			else
			{
				memcpy(buffer + oft[fd].offset, oft[fd].buffer, leftover);
			}
			
			transferred += leftover;
			oft[fd].offset = leftover;
			oft[fd].spaceInBuffer -= leftover;
		}
		else
		{
			memcpy(buffer, oft[fd].buffer + oft[fd].offset, count);
			transferred += count;
			oft[fd].offset = oft[fd].offset + count;
			oft[fd].spaceInBuffer -= count;
		}
		return transferred;
	}
	
void b_close (b_io_fd fd)
	{
		oft[fd].linuxfd = -1;
		free(oft[fd].buffer);
		oft[fd].buffer = NULL;
		oft[fd].offset = 0;
		close(oft[fd].linuxfd);
	}