/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* GitHub UserID: Ninjaruss
* Project: Assignment 4 – Word Blast
*
* File: linkedlist.c
*
* Description:
* Implements a linked list data structure. Here, nodes are words that contain a frequency and word text.
*
* Basic Reference:
* https://www.tutorialspoint.com/learn_c_by_examples/linked_list_search_program_in_c.htm
*
* Quick Sort Reference:
* https://www.geeksforgeeks.org/quicksort-on-singly-linked-list/
* https://tutorialspoint.dev/algorithm/sorting-algorithms/quicksort-on-singly-linked-list
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

// Inserts a new word into the linked list
Word * newWord(char * _word)
{
    // printf("new word: %s\n", _word);
    Word * currentWord = NULL;
    currentWord = (Word*) malloc(sizeof(Word));
    currentWord -> text = (char*) malloc(strlen(_word)+1);
    strcpy(currentWord -> text, _word);
    currentWord -> freq = 1;
    currentWord -> nextWord = NULL;
    return currentWord;
}

// Adds a word into the linked list; if it exists, add 1 to frequency
void addWord(Word * head, char * _word)
{
    Word * currentWord = NULL;
    Word * last = NULL;

    currentWord = head;
    while (currentWord != NULL)
    {
        int code = strcmp(_word, currentWord -> text);
        if (code == 0)
        {
            // printf("word: %s with %d freq.\n", currentWord -> text, currentWord -> freq);
            currentWord -> freq += 1;
            return;
        }
        last = currentWord; 
        currentWord = currentWord -> nextWord;
    }
    // Add new word, if not in list
    last -> nextWord = newWord(_word);
}

Word * paritionLast(Word * start, Word * end) 
{ 
    if(start == end || start == NULL || end == NULL) 
    {
        return start; 
    }

    Word * currentWord = start;
    Word * pivot_prev = start;   
    int pivot = end -> freq;  
      
    // iterate till one before the end,  
    // no need to iterate till the end  
    // because end is pivot 
    int temp;
    while (start != end) 
    { 
        if (start == NULL)
        {
            return pivot_prev;
        }

        if (start -> freq < pivot) 
        {  
            // keep tracks of last modified item 
            pivot_prev = currentWord;
            temp = currentWord -> freq;
            currentWord -> freq = start -> freq;  
            start -> freq = temp;  
            currentWord = currentWord -> nextWord;  
        } 
        start = start -> nextWord;  
    } 
      
    // swap the position of curr i.e. 
    // next suitable index and pivot 
    temp = currentWord -> freq;
    currentWord -> freq = pivot;  
    end -> freq = temp;
    // return one previous to current 
    // because current is now pointing to pivot 
    return pivot_prev; 
} 

// Quick sorts the list by frequency
void sortWords(Word * start, Word * end) 
{ 
    if (start == NULL || start -> text == end -> text) 
    {
        return;
    }

    // split list and partion recurse 
    Word * pivot_prev = paritionLast(start, end);
    sortWords(start, pivot_prev); 
      
    // if pivot is picked and moved to the start, 
    // that means start and pivot is same  
    // so pick from next of pivot 
    if (pivot_prev != NULL && pivot_prev == start)
    {
        sortWords(pivot_prev -> nextWord, end); 
    }
    // if pivot is in between of the list, 
    // start from next of pivot,  
    // since we have pivot_prev, so we move two nodes 
    else if(pivot_prev != NULL && pivot_prev -> nextWord != NULL)
    {
        Word * nextNext = pivot_prev -> nextWord -> nextWord;
        // printf("nextWord: %s\n", pivot_prev -> nextWord -> text);
        sortWords(pivot_prev -> nextWord -> nextWord, end);
    }
}

// Gets last existing word in the list
Word * getLastWord(Word * head) 
{ 
    Word * currentWord = head;
    Word * nextWord = currentWord -> nextWord;

    //&& cur->next != NULL
    while (nextWord != NULL)
    {
        currentWord = nextWord;
        nextWord = currentWord -> nextWord;
    }

    printf("Last node is %s\n", currentWord -> text);
    return currentWord;
} 

/* Checks whether the word is present in linked list */
int searchWord(Word * head, char * _word) 
{ 
    Word * currentWord = head;  // Initialize current 
    while (currentWord != NULL) 
    { 
        // printf("w1: %s | W2: %s\n", currentWord -> text, _word);
        if (strcmp(currentWord -> text, _word) == 0)
        {
            return 1;
        }
        currentWord = currentWord -> nextWord; 
    } 
    return 0; 
}

// Prints the first 10 words in the list (starting from head)
void printFirstTen(Word * head) 
{ 
    Word * currentWord = head;

    int index = 1;
    while (currentWord != NULL && index <= 10) 
    { 
        char * text = currentWord -> text;
        int freq = currentWord -> freq;
        printf("Number %d is %s with a count of %d.\n", index, text, freq); 
        currentWord = currentWord -> nextWord;
        index++;
    }
} 

// Frees all words in the list from memory
void freeList(Word * head)
{
   Word * tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head -> nextWord;
       free(tmp);
    }
}