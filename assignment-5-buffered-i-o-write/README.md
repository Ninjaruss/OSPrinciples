# CSC415-Assignment-5

**Assignment 5 – Buffered I/O - Continued - Buffered Write**

Welcome to your next homework assignment.  

Let me remind you of the expectations.  Code should be neat, well documented.  Variables should have meaningful names and be in a consistent format (I do not care if you use camelCase or under_scores in variables but be consistent.  In addition, each file should have a standard header as defined below.

```
/**************************************************************
* Class:  CSC-415-0# FALL 2020
* Name:
* Student ID:
* Project: <project name, like Assignment 1 – Simple Shell
*
* File: <name of this file>
*
* Description:
*
**************************************************************/
```

This is an INDIVIDUAL assignment.  You can (and should) work in groups to research how to do the assignment, but each person should code their own version and make their own submission.

This assignment is to get you used to using System Calls (API’s) and to handle buffered IO where you do the buffering.

You will see in this assignment that you may question why you are doing operations the way specified here in the assignment.  This is because we are writing a buffering I/O and a test program for the buffered I/O.  We will see later in the course how important this is.

This builds on assignment 2 (the buffered read assignment). In this assignment you are adding `b_write`.  The prototypes for these functions must be in a file called `b_io.h` (provided).

You will be just writing one function and modifying another of the functions in `b_io.c` from your Assignment 2.  If you feel your assignment 2 is good, please use that, otherwise I have provided a version of `b_io.c` for you to start with.

```
	int b_open (char * filename, int flags);		//modified to accept flags
	int b_write (int fd, char * buffer, int count);
```

Your functions here will only use Linux I/O APIs.  i.e. read, write, open, close (do not use the c library file functions).

The `b_open` should return a integer file descriptor (a number that you can track the file).  You may want to also allocate the 512 byte buffer you will need for read operations here.  Make sure though you know how to track the buffer for each individual file. You can return the same error code you receive if the Linux open returns an error.

The `b_write` takes a file descriptor, a buffer and the number of bytes to be written.  The Operation of your `b_write` function must only write 512 bytes chunks at a time to the operating system from your own buffer, you will need to copy the appropriate bytes from the caller's buffer to your buffer.  This means you may not even need to do a write to the actual file if your buffer is not yet full.  Or, it may mean that you have a full buffer to write, then additional bytes to buffer from the caller’s buffer.  
The return value is the number of bytes you have transferred from the caller’s buffer.  
Hint:  You may also need to track between calls where in the buffer you left off.


Limits:  You can assume no more than 20 files open at a time.

Your main program is provided for you, please rename the file from `Bierman_Robert_HW5_main.c` to the appropriate naming for you.
You are provided the file DecOfInd.txt as the file to read.  The output file is nicely formatted in 75 or less character lines.
The file ProvidedClean.txt provided is the correct output.

You should submit your source code file(s), Makefile, and a copy of Clean.txt (output file) along with a short writeup in PDF format that includes a description of what you did and the compilation and execution output from your program in GitHub, and the PDF in iLearn.

Your main program filenames should be `<lastname>_<firstname>HW<#>_<optional>.<proper extension>`


Rubric
| Grade Item                    | Grade Points                                  |
|:------------------------------|----------------------------------------------:|
| Standard Header               |   2                                           |
| Command Arguments             |   5 (free points, since main is provided)     |
| Proper use of open and malloc |  10                                           |
| Proper use of write           |  10                                           |
| Proper buffering              |  20                                           |
| Proper use of close and free  |   5                                           |
| Correct Output                |   5                                           |
| Code Comments                 |   5                                           |
| Writeup                       |   8 (Description, Compilation, Sample Output) |
