/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* GitHub UserID: Ninjaruss
* Project: Assignment 4 – Word Blast
*
* File: hashtable.c
*
* Description:
* Implements a hashed table data structure.
*
* Hash Table Reference:
* https://medium.com/@bennettbuchanan/an-introduction-to-hash-tables-in-c-b83cbf2b4cf6
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

// Hashes a word
unsigned int hash(const char * token, unsigned int size)
{
    unsigned long int value = 0;
    unsigned int token_len = strlen(token);

    // do several rounds of multiplication
    for (int i = 0; i < token_len; ++i) 
    {
        value = (value * 37) + token[i];
    }

    // make sure value is 0 <= value < TABLE_SIZE
    value = value % size;

    return value;
}

// Initializes a new word table
WordTable * createTable(unsigned int size)
{
    WordTable * wordTable;

    if (size < 1)
    {
        return NULL;
    }

    wordTable = malloc(sizeof(WordTable));
    if (wordTable == NULL)
    {
        return NULL;
    }

    wordTable -> words = (Word **) malloc(size * sizeof(Word));
    if (wordTable -> words == NULL)
    {
        return NULL;
    }

    memset(wordTable -> words, 0, size * sizeof(Word));
    wordTable -> size = size;

    return wordTable;
    
}

// Gets the word frequency if it exists in the table
char * getWordFreq(WordTable * wordTable, const char * word)
{
    char * wordCopy;
    unsigned int i;
    Word * tmp;

    if (wordTable == NULL) 
    {
        return NULL;
    }

    wordCopy = strdup(word);
    i = hash(word, wordTable -> size);
    tmp = wordTable -> words[i];

    while (tmp != NULL) 
    {
        if (strcmp(tmp -> token, wordCopy) == 0) 
        {
            break;
        }
        tmp = tmp -> nextWord;
    }
    free(wordCopy);

    if (tmp == NULL) 
    {
        return NULL;
    }

    return tmp -> freq;
}

// Sets the word's frequency in the table; passes word to handler
int setWord(WordTable * wordTable, const char * token, const char * freq)
{
    Word * currentWord;

    if (wordTable == NULL)
    {
        return 1;
    }

    currentWord = malloc(sizeof(Word));
    if (currentWord == NULL)
    {
        return 1;
    }

    currentWord -> token = strdup(token);
    currentWord -> freq = strdup(freq);

    wordHandler(wordTable, currentWord, freq);
    return 0;
}

// Handles adding word to table; otherwise will set a new freq for the word
void wordHandler(WordTable * wordTable, Word * word, const char * freq)
{
    unsigned int i = hash(word -> token, wordTable -> size);
    Word * tmp = wordTable -> words[i];

    if (wordTable -> words[i] != NULL)
    {
        tmp = wordTable -> words[i];
        while (tmp != NULL)
        {
            if (strcmp(tmp -> token, word -> token) == 0)
            {
                break;
            }
            tmp = tmp -> nextWord;
        }

        // If no found matches, add word to the table
        if (tmp == NULL)
        {
            word -> nextWord = wordTable -> words[i];
            wordTable -> words[i] = word;
        }
        // If found match, set new freq
        else
        {
            free(tmp -> freq);
            tmp -> freq = strdup(word -> freq);
            free(word -> freq);
            free(word -> token);
            free(word);
        }
    }
    else
    {
        word -> nextWord = NULL;
        wordTable -> words[i] = word;
    }
}

// Frees the word table from memory
void freeTable(WordTable * wordTable)
{
    Word * tmp;
    unsigned int i;

    if (wordTable == NULL) 
    {
        return;
    }

    for (i = 0; i < wordTable -> size; ++i) 
    {
        if (wordTable -> words[i] != NULL) 
        {
            // Free each word entry in the table from memory
            while (wordTable -> words[i] != NULL) 
            {
                tmp = wordTable -> words[i] -> nextWord;
                free(wordTable -> words[i] -> token);
                free(wordTable -> words[i] -> freq);
                free(wordTable -> words[i]);
                wordTable -> words[i] = tmp;
            }
            // Free the word table from memory
            free(wordTable -> words[i]);
        }
    }

    free(wordTable -> words);
    free(wordTable);
}

// Return the top ten most frequent words from the word table
void getTopTen(WordTable * wordTable, Word topTenWords[])
{
    // Keep track of the current max and compare to the previous one; previousMax initialized for initial word
    int currentMax = 0;
    int previousMax = 10000000;
    int frequency;

    // Node for most frequent word; will be put into array of top ten frequent words
    Word insertWord;

    // Insert most frequent word from the table into the array, then repeat for next index 
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < wordTable -> size; j++)
        {
            Word * currentWord = wordTable -> words[j];

            // Skip rest of loop if no current word
            if (currentWord == NULL)
            {
                continue;
            }

            // printf("[%d] %s\n", atoi(currentWord -> freq), currentWord -> token);
            // Convert the char pointer to int to compare frequencies
            frequency = atoi(currentWord -> freq);

            // Check if current frequency is greater than current max; if so, set as new current max 
            if (frequency > currentMax && frequency < previousMax)
            {
                currentMax = frequency;
                insertWord.token = currentWord -> token;
                insertWord.freq = currentWord -> freq;
                insertWord.nextWord = NULL; 
            }
        }
        // printf("insertWord: %s | previousMax: %d | currentMax: %d\n", insertWord.token, previousMax, currentMax);
        // Set new previous max and reset current
        topTenWords[i] = insertWord;
        previousMax = currentMax;
        currentMax = 0;
    }
}