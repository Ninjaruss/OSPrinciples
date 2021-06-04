# CSC415-Assignment-1
Assignment 1 – Command Line Arguments
Welcome to your First homework assignment.  

Let me set some of the expectations.  Code should be neat, well documented.  Variables should have meaningful names and be in a consistent format (I do not care if you use camelCase or under_scores in variables but be consistent.  

All filenames should be `<lastname>_<firstname>_HW<#>_<optional>.<proper extension>`, except Makefile.

In addition, each file should have a standard header as defined below.  Make sure to put in your section number, your name, your student ID, a proper project name, filename, and description of the project.

```
/**************************************************************
* Class:  CSC-415-0# Fall 2020
* Name:
* Student ID:
* Project: <project name>, Assignment 1 – Command Line Arguments
*
* File: <name of this file>
*
* Description:
*
**************************************************************/
```
This is an INDIVIDUAL assignment.  You can (and should) work in groups to research how to do the assignment, but each person should code their own version and make their own submission.

This program is to demonstrate the usage of command line arguments in a C program.  You will write this to run in the Linux virtual machine.  The program should display the number of arguments from the command line and list each one on the console. 

Here is a sample execution:
 
```
student:~/CSC415$ make run RUNOPTIONS="Hello, these are overridden options 3 6 9"
gcc -c -o bierman_robert_hw1_main.o bierman_robert_hw1_main.c -g -I. 
gcc -o bierman_robert_hw1_main bierman_robert_hw1_main.o -g -I.  
./bierman_robert_hw1_main Hello, these are overridden options 3 6 9

There were 9 arguments on the command line.
Argument 00: 	./bierman_robert_hw1_main
Argument 01: 	Hello,
Argument 02: 	these
Argument 03: 	are
Argument 04: 	overridden
Argument 05: 	options
Argument 06: 	3
Argument 07: 	6
Argument 08: 	9
student:~/CSC415$
```

You should submit your source code file(s), Makefile along with a short writeup in PDF format into your GIT repository and submit the PDF into the assignment in iLearn.  Your write-up should include a description of the project and what you did, issues you had, how you overcame the issues and the compilation and execution output from your program (screen shots embedded in the PDF document. Your execution output should include commands with the command-line arguments.  

All filenames should be `<lastname>_<firstname>_HW<#>_<optional>.<proper extension>`

```
# File: Makefile

ROOTNAME=bierman_robert_HW
HW=1
FOPTION=_main
RUNOPTIONS=Hello World
CC=gcc
CFLAGS= -g -I.
LIBS =
DEPS = 
OBJ = $(ROOTNAME)$(HW)$(FOPTION).o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(ROOTNAME)$(HW)$(FOPTION): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)  $(LIBS)

clean:
	rm *.o $(ROOTNAME)$(HW)$(FOPTION)

run: $(ROOTNAME)$(HW)$(FOPTION)
	./$(ROOTNAME)$(HW)$(FOPTION) $(RUNOPTIONS)
```

Rubric
| Grade Item        | Grade Points                                |
|:------------------|--------------------------------------------:|
| Standard Header   | 1                                           |
| Command Arguments | 15                                          |
| Output Status     | 3                                           |
| Code Comments     | 5                                           |
| Writeup           | 6 (Description, Compilation, Sample Output) |
