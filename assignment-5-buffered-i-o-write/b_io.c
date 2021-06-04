/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* Project: Assignment 5 – Buffered I/O
*
* File: b_io.c
*
* Description: Buffered io module - Now with b_write
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"

#define MAXFCBS 20
#define BUFSIZE 512

typedef struct b_fcb
	{
	int linuxFd;	//holds the systems file descriptor
	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	int flags; // read/write flags
	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].linuxFd = -1; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
int b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].linuxFd == -1)
			{
			fcbArray[i].linuxFd = -2; // used but not assigned
			return i;		//Not thread safe
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
int b_open (char * filename, int flags)
	{
	int fd;
	int returnFd;
	
	if (startup == 0) b_init();  //Initialize our system
	
	// lets try to open the file before I do too much other work
	
	fd = open (filename, flags);
	if (fd  == -1)
		return (-1);		//error opening filename
		
	//Should have a mutex here
	returnFd = b_getFCB();				// get our own file descriptor
										// check for error - all used FCB's
	fcbArray[returnFd].linuxFd = fd;	// Save the linux file descriptor
	//	release mutex
	
	//allocate our buffer
	fcbArray[returnFd].buf = malloc (BUFSIZE);
	if (fcbArray[returnFd].buf  == NULL)
		{
		// very bad, we can not allocate our buffer
		close (fd);							// close linux file
		fcbArray[returnFd].linuxFd = -1; 	//Free FCB
		return -1;
		}

	// Save the flags to distinguish between files with write/read permissions
	fcbArray[returnFd].flags = flags;
		
	fcbArray[returnFd].buflen = 0; 			// have not read anything yet
	fcbArray[returnFd].index = 0;			// have not read anything yet
	return (returnFd);						// all set
	}




// Interface to write a buffer	
int b_write (int fd, char * buffer, int count)
{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	if (fcbArray[fd].linuxFd == -1)		//File not open for this descriptor
		{
		return -1;
		}	
		
	//*** TODO ***:  Write buffered write function to accept the data and # bytes provided
	//               You must use the Linux System Calls and you must buffer the data
	//				 in 512 byte chunks and only write in 512 byte blocks.
	
	// Cache data into linux buffer until it is full
	// Once linux buffer is full, write that data into the linux file
	// Clear out the buffer (flush?)
	// Repeat caching until no more left
	// If the received buffer has no more data left, call the last write after closing?
	int bytesWritten;				// for our writes
	int bytesReturned;			// what we will return
	int part1, part2, part3;	// holds the three potential copy lengths
	int remain = fcbArray[fd].buflen - fcbArray[fd].index;

	part3 = 0;	//only used if count > BUFSIZE
	// Enough space in buffer
	if (remain >= count)  	
	{
		part1 = count;		// completely buffered
		part2 = 0;
	}
	// Not enough space in buffer
	else
	{
		part1 = remain;
		part2 = count - remain;
	}

	if (part1 > 0)	// Copy data from their buffer to mine with size: remain
	{
		memcpy(fcbArray[fd].buf + fcbArray[fd].index, buffer, part1);
		fcbArray[fd].index = fcbArray[fd].index + part1;
	}
		
	// If part2 is greater than zero, then our buffer is full
	if (part2 > 0)
	{
		// Handle special case where user is asking for more than a buffer worth
		if (part2 > BUFSIZE)
		{
			int blocks = part2 / BUFSIZE; // calculate number of blocks they want
			bytesWritten = write(fcbArray[fd].linuxFd, buffer + part1, blocks * BUFSIZE);
			part3 = bytesWritten;
			part2 = part2 - part3;  //part 2 is now < BUFSIZE, or file is exusted

		}
		
		// Special case where the buffer's remaining space has not been initialized
		if (remain == 0)
		{
			fcbArray[fd].buflen = BUFSIZE;
			memcpy(fcbArray[fd].buf + fcbArray[fd].index, buffer + part1 + part3, part2);
			fcbArray[fd].index = fcbArray[fd].index + part2;
		}
		// Write full buffer to the file then reset the buffer
		else
		{
			bytesWritten = write(fcbArray[fd].linuxFd, fcbArray[fd].buf, BUFSIZE);
			fcbArray[fd].index = 0;
			fcbArray[fd].buflen = BUFSIZE;

			if (bytesWritten < part2) // not even enough left to satisfy write
				part2 = bytesWritten;	
			if (part2 > 0)	// memcpy bytesRead
			{
				memcpy(fcbArray[fd].buf + fcbArray[fd].index, buffer + part1 + part3, part2);
				fcbArray[fd].index = fcbArray[fd].index + part2;
			}
		}
	}
	bytesReturned = part1 + part2 + part3;
	return (bytesReturned);
}




// Interface to read a buffer	
int b_read (int fd, char * buffer, int count)
	{
	int bytesRead;				// for our reads
	int bytesReturned;			// what we will return
	int part1, part2, part3;	// holds the three potential copy lengths
	
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	if (fcbArray[fd].linuxFd == -1)		//File not open for this descriptor
		{
		return -1;
		}	
		
	
	// number of bytes available to copy from buffer
	int remain = fcbArray[fd].buflen - fcbArray[fd].index;	
	part3 = 0;				//only used if count > BUFSIZE
	if (remain >= count)  	//we have enough in buffer
		{
		part1 = count;		// completely buffered
		part2 = 0;
		}
	else
		{
		part1 = remain;				//spanning buffer (or first read)
		part2 = count - remain;
		}
				
	if (part1 > 0)	// memcpy part 1
		{
		memcpy (buffer, fcbArray[fd].buf + fcbArray[fd].index, part1);
		fcbArray[fd].index = fcbArray[fd].index + part1;
		}
		
	if (part2 > 0)		//We need to read to copy more bytes to user
		{
		// Handle special case where user is asking for more than a buffer worth
		if (part2 > BUFSIZE)
			{
			int blocks = part2 / BUFSIZE; // calculate number of blocks they want
			bytesRead = read (fcbArray[fd].linuxFd, buffer+part1, blocks*BUFSIZE);
			part3 = bytesRead;
			part2 = part2 - part3;  //part 2 is now < BUFSIZE, or file is exusted
			}				
		
		//try to read BUFSIZE bytes into our buffer
		bytesRead = read (fcbArray[fd].linuxFd, fcbArray[fd].buf, BUFSIZE);
		
		// error handling here...  if read fails
		
		fcbArray[fd].index = 0;
		fcbArray[fd].buflen = bytesRead; //how many bytes are actually in buffer
		
		if (bytesRead < part2) // not even enough left to satisfy read
			part2 = bytesRead;
			
		if (part2 > 0)	// memcpy bytesRead
			{
			memcpy (buffer+part1+part3, fcbArray[fd].buf + fcbArray[fd].index, part2);
			fcbArray[fd].index = fcbArray[fd].index + part2;
			}
			
		}
	bytesReturned = part1 + part2 + part3;
	return (bytesReturned);	
	}
	
// Interface to Close the file	
void b_close (int fd)
{
	// If file has the write flag, execute final write with remaining buffer
	if (fcbArray[fd].flags == 577)
	{
		write(fcbArray[fd].linuxFd, fcbArray[fd].buf, fcbArray[fd].index);
	}

	close (fcbArray[fd].linuxFd);		// close the linux file handle
	free (fcbArray[fd].buf);			// free the associated buffer
	fcbArray[fd].buf = NULL;			// Safety First
	fcbArray[fd].linuxFd = -1;			// return this FCB to list of available FCB's 
}
