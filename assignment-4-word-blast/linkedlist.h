/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* GitHub UserID: Ninjaruss
* Project: Assignment 4 – Word Blast
*
* File: linkedlist.h
*
* Description:
* Header file for linkedlist.c
**************************************************************/

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct Word 
{
   char * text;
   int freq;
   struct Word * nextWord;
} Word;

Word * newWord(char * _word);
void addWord(Word * head, char * _word);

Word * paritionLast(Word * start, Word * end);
void sortWords(Word * start, Word * end);
Word * getLastWord(Word * head);
int searchWord(Word * head, char * _word);
void printFirstTen(Word * head);
void freeList(Word * head);

#endif