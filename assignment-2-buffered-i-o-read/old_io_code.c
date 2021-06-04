/**************************************************************
* Class:  CSC-415-01 Summer 2020
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
#include "stdio.h"
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
	int freeSpace;
	int fileEmpty;
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
		oft[i].freeSpace = -1;
		oft[i].fileEmpty = 1;
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
	//*** TODO ***:  Write open function to return file descriptor
	//               You must use the Linux System Calls (open)
	//				 You may want to allocate the buffer here as well
	//				 But make sure every file has its own buffer
		if (initialized == 0) oft_init();

		// Check for available spot and return index if available
		int index = b_getIndex();
		if (index < 0){
			return -1;
		}

		oft[index].buffer = malloc(B_CHUNK_SIZE);
		if (oft[index].buffer == NULL)
		{
			printf("malloc failed.");
			return -1;
		}
		oft[index].linuxfd = index;
		oft[index].bfd = open(filename, O_RDWR);
		oft[index].fileEmpty = 0;
		//printf("[bopen] arr bfd: %d\n", oft[index].bfd);
		//printf("[bopen] arr linuxfd: %d\n", oft[index].linuxfd);
		return oft[index].linuxfd;
	}
	
int b_read (b_io_fd fd, char * buffer, int count)
	{
	//*** TODO ***:  Write buffered read function to return the data and # bytes read
	//               You must use the Linux System Calls (read) and you must buffer the data
	//				 in 512 byte chunks. i.e. your linux read must be in B_CHUNK_SIZE
		int x;
		int trueCount = count; // count to buffer
		int totalTransferred = 0; // data copied and moved into buffer

		if (oft[fd].freeSpace == -1)
		{
			int limitOffset = 0;
			// If request count is greater than chunk size limit, read directly to buffer
			if (B_CHUNK_SIZE < count)
			{
				int loop = 0;
				while (trueCount > B_CHUNK_SIZE)
				{
					x = read(oft[fd].bfd, buffer, B_CHUNK_SIZE);
					trueCount -= B_CHUNK_SIZE;
					loop += 1;
				}
				// printf("[-----\nbuffer2a: %s\n-----]\n", oft[fd].buffer);
				x = read(oft[fd].bfd, oft[fd].buffer, trueCount);
				limitOffset = B_CHUNK_SIZE * loop;
			}
			else
			{
				x = read(oft[fd].bfd, oft[fd].buffer, B_CHUNK_SIZE);
			}
			
			// x = read(oft[fd].bfd, oft[fd].buffer, B_CHUNK_SIZE);
			// printf("truecount: %d\n", trueCount);
			memcpy(buffer + limitOffset, oft[fd].buffer, trueCount);
			totalTransferred += trueCount;
			oft[fd].offset = oft[fd].offset + trueCount;
			oft[fd].freeSpace = x - trueCount;

			if (oft[fd].offset > B_CHUNK_SIZE){
				oft[fd].offset = oft[fd].offset - B_CHUNK_SIZE;
			}
			if (oft[fd].freeSpace < 0){
				int subtract = oft[fd].freeSpace * -1;
				oft[fd].freeSpace = B_CHUNK_SIZE - subtract;
			}
			return count;totalTransferred += trueCount;
		}

		/*
		trueCount = count;
		int limitOffset = 0;
		if (B_CHUNK_SIZE < count){
			int loop = 0;
			while (trueCount > B_CHUNK_SIZE)
			{
				int offset = B_CHUNK_SIZE * loop;
				memcpy(buffer + offset, oft[fd].buffer + oft[fd].offset, x);
				x = read(oft[fd].bfd, oft[fd].buffer, trueCount);
				// x = read(oft[fd].bfd, buffer + offset, B_CHUNK_SIZE);
				trueCount -= B_CHUNK_SIZE;
				loop += 1;
			}
			x = read(oft[fd].bfd, oft[fd].buffer, trueCount);
			limitOffset = B_CHUNK_SIZE * loop;
		}
		*/

		// If not enough space, read more data into buffer
		printf("freespace: %d\n", oft[fd].freeSpace);
		printf("count: %d\n", count);
		

		if (oft[fd].freeSpace < trueCount)
		{
			printf("1\n");

			// printf("--freespace < count--\n");
			x = oft[fd].freeSpace;
			printf("readx: %d\n", x);
			int leftover = count - x;;
			
			printf("leftover: %d\n", leftover);
			
			memcpy(buffer, oft[fd].buffer + oft[fd].offset, x);
			
			totalTransferred += x;
			oft[fd].offset = x;
			
			printf("[-----\nbuffer1: %s\n-----]\n", oft[fd].buffer);
			if (oft[fd].fileEmpty == 0)
			{
				x = read(oft[fd].bfd, oft[fd].buffer, B_CHUNK_SIZE);
			}

			if (x < B_CHUNK_SIZE && oft[fd].fileEmpty == 0){
				oft[fd].fileEmpty = 1;
				int leftover2 = B_CHUNK_SIZE - x;
				memset(oft[fd].buffer + x, 0, leftover2 * sizeof(oft[fd].buffer));
				//oft[fd].offset = leftover2;
				// oft[fd].freeSpace = x - leftover2;

				printf("[-----\nbuffer3: %s\n-----]\n", oft[fd].buffer);
			}

			//printf("fe: %d\n", oft[fd].fileEmpty);
			// Need to offset the current buffer
			
			
			
			
			//printf("xfinal: %d\n", x);

			// printf("[-----\nbuffer2: %s\n-----]\n", oft[fd].buffer);
			
			memcpy(buffer + oft[fd].offset, oft[fd].buffer, leftover);
			
			
			totalTransferred += leftover;
			oft[fd].offset = leftover;
			oft[fd].freeSpace = x - count;
		}
		// Has enough space in the current read buffer
		else
		{
			printf("2\n");
			x = oft[fd].freeSpace;

			memcpy(buffer, oft[fd].buffer + oft[fd].offset, count);
			totalTransferred += count;
			oft[fd].offset = oft[fd].offset + count;
			oft[fd].freeSpace = x - count;
		}
		// printf("totalSent: %d\n", totalTransferred);
		return totalTransferred;
	}
	
void b_close (b_io_fd fd)
	{
	//*** TODO ***:  Close the file and release the buffer
	//				 You must use Linux System Calls (close)
		oft[fd].linuxfd = -1;
		free(oft[fd].buffer);
		oft[fd].buffer = NULL;
		oft[fd].offset = 0;
		close(oft[fd].linuxfd);
	}