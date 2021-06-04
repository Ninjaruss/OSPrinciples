/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* GitHub UserID: Ninjaruss
* Project: Assignment 4 – Word Blast
*
* File: Azucenas_Russell_HW4_main.c
*
* Description:
* Program counts words that are of size 6 or more characters and displays the top 10 frequent words. Implements Multi-threading.
**************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "hashtable.h"
// #include "linkedlist.h"

// Start and end points for reading a file chunk 
// (unsigned for largest range and non-negative)
typedef struct {
    unsigned long start;
    unsigned long end;
} fileChunk;

// Word buffer length. Words are typically short; largest word in major dict is 45
int maxWordBuffer = 64;
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";
char * filePath;

// Need to declare functions for use in main early
void * processFile(void * _chunk);
char * readFile(long start, long end);

// Hash table containing words with frequencies
WordTable * wordTable;
// Size of word table; needs to be large to prevent hash collisions
int tableSize = 10000000;

// Mutex locks for newWord and addWord
// newWordLock for adding new word 
// addFreqLock for adding a freq to an existing word
pthread_mutex_t newWordLock;
pthread_mutex_t addFreqLock;

int main (int argc, char *argv[])
{
    //***TO DO***  Look at arguments, open file, divide by threads
    //             Allocate and Initialize and storage structures
    int threadCount;
    long fileSize;
    long chunkSize;
    // Need to keep track of file descriptor (for null checks and fseek)
    int fd;

    // Number of args must be correct size
    if (argc == 3)
    {
        filePath = argv[1];
        threadCount = atoi(argv[2]);
    }
    else
    {
        printf("Incorrect arg format for passed args.\n");
        return -1;
    }

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish

    // Initialize word table
    wordTable = createTable(tableSize);

    // Initialize mutex locks
    if (pthread_mutex_init(&newWordLock, NULL) != 0) 
    {
        printf("\n mutex init has failed\n");
        return 1;
    }

    if (pthread_mutex_init(&addFreqLock, NULL) != 0) 
    {
        printf("\n mutex init has failed\n");
        return 1;
    }
    
    // Allocate space for each thread
    pthread_t * threads = malloc((sizeof(pthread_t) * threadCount * 20) + 1);
    if (threads == NULL)
    {
        printf("Failed to allocate space for threads.\n");
        return -1;
    }

    // Open the file with read permission; return -1 if failed to open
    fd = open(filePath, S_IRUSR);
    if (fd >= 0)
    {
        // lseek returns resulting offset; therefore is an indicator for the file size
        fileSize = lseek(fd, 0, SEEK_END);
        // Must close to keep open/close calls localized for threads later on
        close(fd);
    }
    else
    {
        printf("Failed to open file.\n");
        return -1;
    }
    chunkSize = fileSize/threadCount;
    int remainder = threadCount - 1;

    // Set file chunk data according to index then create a thread for each chunk to process
    fileChunk* fileChunk;
    for (int i = 0; i < threadCount; i++)
    {
        fileChunk = malloc(sizeof(fileChunk));
        if (fileChunk == NULL)
        {
			printf("Failed to allocate space for a file chunk.\n");
			return -1;
		}

        fileChunk -> start = i * chunkSize;
        fileChunk -> end = (i * chunkSize) + chunkSize;

        if (i == (threadCount - 1))
        {
            fileChunk -> end += remainder;
        }

        int code = pthread_create(&threads[i], NULL, processFile, (void*) fileChunk);
        if (code != 0)
        {
            printf("Error occurred while creating thread.\n");
        }
    }


    // Wait for each thread to finish
    for (int i = 0; i < threadCount; i++)
    {
        int code = pthread_join(threads[i], NULL);
        if (code != 0)
        {
            printf("Error occurred while joining thread.\n");
        }
        else
        {
            // printf("Thread #%d successfully completed.\n", i + 1);
        }
    }

    // ***TO DO *** Process TOP 10 and display
    // Create an array of words
    Word topTenArray[10];
    // Put top ten frequent words into the array
    getTopTen(wordTable, topTenArray);

    printf("Printing top 10 words 6 characters or more. (case sensitive)\n");
    for (int i = 0; i < 10; i++)
    {
        char * token = topTenArray[i].token;
        int freq = atoi(topTenArray[i].freq);
        printf("Number %d is %s with a count of %d\n", i + 1, token, freq);
    }
    printf("\nCompleted using %d threads.\n", threadCount);
    
    

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************


    // ***TO DO *** cleanup
    free(threads);
    free(fileChunk);
    freeTable(wordTable);
    threads = NULL;
    pthread_mutex_destroy(&newWordLock);
    pthread_mutex_destroy(&addFreqLock);
}

// Processes a chunk of the file by reading text then counting frequency 
void * processFile(void * _chunk)
{
    fileChunk* chunk = (fileChunk*) _chunk;
    // '->' operator used to access data from struct pointer
    unsigned long start = chunk -> start;
    unsigned long end = chunk -> end;
    char * fileText = readFile(start, end);

    char * currentToken;
    // Initialize storage for tokens
    char ** tokenArray = malloc(maxWordBuffer * sizeof(char*));
    if (tokenArray == NULL)
    {
        printf("Failed to allocate space for tokenArray.");
        exit(EXIT_FAILURE);
    }
    char * oldText;

    int index = 0;
    // Buffer may change size in order to account for overflow of tokens
    int bufferSize = maxWordBuffer;

    // Divide text into tokens and move valid tokens into array
    while ((currentToken = strtok_r(fileText, delim, &oldText)))
    {
        // Add token to array if it has 6 or more characters
        int charCount = strlen(currentToken);
        if (charCount > 5)
        {
            tokenArray[index] = currentToken;
            index += 1;
        }

        // Need to check if token array has enough space for the next token
        if (index >= bufferSize)
        {
            bufferSize += maxWordBuffer;
            tokenArray = realloc(tokenArray, bufferSize * sizeof(char*));
            if (tokenArray == NULL)
            {
                printf("Failed to reallocate space for token array.\n");
                exit(EXIT_FAILURE);
            }
        }
        fileText = oldText;
    }
    // Last position index is empty
    tokenArray[index] = NULL;

    // Count and store frequencies of each token
    index = 0;
    while(1)
    {
        // Exit loop if no more tokens
        if (tokenArray[index] == NULL)
        {
            break;
        }

        char * currentToken = tokenArray[index];

        pthread_mutex_lock(&newWordLock);
        char * checkToken = getWordFreq(wordTable, currentToken);
        // Check if the token already exists in table; if it doesn't exist, add new word to table 
        if (checkToken == NULL)
        {
            setWord(wordTable, currentToken, "1");
            pthread_mutex_unlock(&newWordLock);
        }
        // Increase frequency of token since it is in the table
        else
        {
            pthread_mutex_unlock(&newWordLock);

            pthread_mutex_lock(&addFreqLock);
            // Convert the frequency char pointer to int then increase by 1
            int count = atoi(getWordFreq(wordTable, currentToken)) + 1;
            
            // Reformats the count into a char
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", count);

            // Set the new incremented frequency of the current token in the word table
            int code = setWord(wordTable, currentToken, buffer);
            if (code == 1){
                printf("Failed to set word in table.\n");
                exit(EXIT_FAILURE);
            }
            
            pthread_mutex_unlock(&addFreqLock);
        }
        index += 1;
    }
    free(tokenArray);
}

// Read the file from a specific section
char * readFile(long start, long end)
{
    char * fileText;
    int chunkSize = end - start;

    int fd = open(filePath, S_IRUSR);
    if (fd >= 0)
    {
        // Set the file's offset to the start of the file chunk
        lseek(fd, start, SEEK_SET);
        fileText = malloc(chunkSize);
        if (fileText == NULL)
        {
            perror("Failed to allocate space for file text buffer.\n");
        }

        read(fd, fileText, chunkSize);
        close(fd);
    }
    else
    {
        printf("Failed to open file.\n");
    }

    return fileText;
}