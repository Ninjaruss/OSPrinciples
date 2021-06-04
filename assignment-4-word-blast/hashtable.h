/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* GitHub UserID: Ninjaruss
* Project: Assignment 4 – Word Blast
*
* File: hashtable.h
*
* Description:
* Header file for hash table.
**************************************************************/
#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct Word 
{
    char * token;
    char * freq;
    struct Word * nextWord;
} Word;

typedef struct 
{
    unsigned int size;
    Word ** words;
} WordTable;

unsigned int hash(const char * token, unsigned int size);
WordTable * createTable();
char * getWordFreq(WordTable * wordTable, const char * word);
int setWord(WordTable * wordTable, const char * token, const char * freq);
void wordHandler(WordTable * wordTable, Word * word, const char * freq);
void freeTable(WordTable * wordTable);
void getTopTen(WordTable * wordTable, Word topTenWords[]);

#endif