# CSC415-Assignment-2

**Assignment 2 – Buffered I/O**

Welcome to your second homework assignment.  

Let me remind you of the expectations.  Code should be neat, well documented.  Variables should have meaningful names and be in a consistent format (I do not care if you use camelCase or under_scores in variables but be consistent.  In addition, each file should have a standard header as defined below.

```
/**************************************************************
* Class:  CSC-415-0# Summer 2020
* Name:
* Student ID:
* GitHub UserID:
* Project: Assignment 2 – Buffered I/O read
*
* File: <name of this file>
*
* Description:
*
**************************************************************/
```

This is an INDIVIDUAL assignment.  You can (and should) work in groups to research how to do the assignment, but each person should code their own version and make their own submission.

This assignment is to get you used to using Linux System Calls (API’s) and to handle buffered IO where you do the buffering.  This means using open, close and read and not any other form of these API's.  i.e. do not use fread.

You will see in this assignment that you may question why you are doing operations the way specified here in the assignment.  This is because we are writing a buffering I/O and a test program for the buffered I/O.  We will see later in the course how important this is.

You are to create a set of routines in a file called `b_io.c` (don’t forget to add this file in the Makefile list).  The prototypes for these functions must be in a file called `b_io.h` (provided).  The Makefile can be copied from assignment one.

You will be just writing three functions in `b_io.c`

```
	b_io_fd b_open (char * filename);
	int b_read (b_io_fd fd, char * buffer, int count);
	void b_close (b_io_fd fd);
```

Your functions here will only use Linux I/O APIs.  i.e. read, open, close (do not use the c library file functions).

The `b_open` should return a integer file descriptor (a number that you can track the file).  You may want to also allocate the 512 byte buffer you will need for read operations here.  Make sure though you know how to track the buffer for each individual file. You can return the same error code you receive if the Linux open returns an error.

The `b_read` takes a file descriptor, a buffer and the number of bytes desired.  The Operation of your `b_read` function must only read 512 bytes chunks at a time from the operating system into your own buffer, you will then copy the appropriate bytes from your buffer to the caller’s buffer.  This means you may not even need to do a read of the actual file if your buffer already has the data needed.  Or, it may mean that you have some bytes in the buffer, but not enough and have to transfer what you have, read the next 512 bytes, then copy the remaining needed bytes to the caller’s buffer.  
The return value is the number of bytes you have transferred to the caller’s buffer.  When it is positive but less than the request, it means you have reached the end of file.
Hint:  You may also need to track between calls where in the buffer you left off.

You also need to be able to handle if the read request is greater than 512, meaning that you may have to directly fill the caller's buffer from a 512 byte read (no need to buffer) then buffer just any amount needed to complete the caller's read request.

The `b_close` should close the file on the Linux side, and free and resources you were using.

You can write additional helper routines as needed.

Limits:  You can assume no more than 20 files open at a time. (i.e. you need to ensure that multiple files can be open at one time, so the buffer you have for a file can not be global, but must be associated with that open file.

Your main program (separate file, starting template for main is provided) should use the command line arguments for the file to `b_open`. 
So your main program should open the file using `b_open`, read exactly 80 characters at a time from the file using `b_read`, print those 80 characters to the screen (ending in a newline character), and loop until you have read the entire file, then `b_close` the file and exit.

You are provided the file DecOfInd.txt as the file to read and display.

You should submit your source code file(s) and Makefile, along with a short writeup in PDF format that includes a description of what you did and the compilation and execution output from your program in GitHub, and the PDF in iLearn.

Your main program filenames should be `<lastname>_<firstname>HW<#>_<optional>.<proper extension>`


Rubric
| Grade Item                    | Grade Points                                  |
|:------------------------------|----------------------------------------------:|
| Standard Header               |   2                                           |
| Command Arguments             |   5                                           |
| Proper use of open and malloc |  10                                           |
| Proper use of read            |  10                                           |
| Proper buffering              |  20                                           |
| Proper use of close and free  |   5                                           |
| Correct Output                |   5                                           |
| Code Comments                 |   5                                           |
| Writeup                       |   8 (Description, Compilation, Sample Output) |
