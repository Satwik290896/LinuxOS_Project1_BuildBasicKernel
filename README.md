# f22-hmwk1-Satwik290896
This project is the first assignment or HW-1 of Operating Systems class at Columbia University

This project will heavily focus on making the code appear neat with upto 3 levels of indentations (Only tabs)

# Part-1

Part-1 of this project is to write manual "shell.c" in which exectable commands of "/bin/sh" will be executed along with built-in commands "cd" and "exit". This is a replication of what the actual terminal in Linux does. Project uses fork() to create a child process to execute the commands. Shell.c can terminate for the following reasons.

1. Memory Allocation Failures
2. Child process creation failures
3. exit()
4. waitpid() failure

Go to Part1 folder and execute the following commands to run shell.c
> make clean

> make

> ./w4118_sh

##Contents of part1
1. Makefile - to link all the source codes
2. shell.c - Source code
3. shell.h - Header file
4. w4118.sh - This can be recreated by running "make" command. This is a binary executed file assosciated with shell.c. Running this file, will run the source code.

# Part-2

Part-2 of this projet is to write manual "shell2.c" likewise part1 - but, shell2.c is written with direct systemcalls without the use of indirect C library functions that calls systemcall internally. Especially, in shell2.c, we need to play with pointer allocations and de-allocations perfectly so that - we don't see any wierd buffer overflows. Shell2.c can terminate for the following reasons.

1. Memory Allocation Failures
2. Child process creation failures
3. exit()
4. waitpid() failure
5. write() failures
6. read() failures




Go to Part2 folder and execute the following commands to run shell2.c
> make clean

> make

> ./w4118_sh2

##Contents of part2
1. Makefile - to link all the source codes
2. shell2.c - Source code
3. shell2.h - Header file
4. w4118.sh2 - This can be recreated by running "make" command. This is a binary executed file assosciated with shell2.c. Running this file, will run the source code.


# Part-3

Part-3 of this project is to create floppy.flp file so that when the device bootsup, BIOS will load the contents of the floppy.flp file instead of the Linux Operating System. floppy.flp contains contents such that - it will display "hello, world" at the center of VGA console with White background and black foreground. A screenshot of it is provided below (run on x86 processor)

To re-create the floppy.flp file, go to the part3 folder and run bash command on x86 processor. For ARM processor, the commands will vary and you may need to create a diffeent Shell script for it
> bash run.sh

When you read bash run.sh, you may see that the two main files to create floppy.flp are -
1. entry.S - Contains alll the assemly code that is responsible to make BIOS read both the floppy.flp file and segment base. 
2. main.c - Segment base is set here as - "0xb800" and the code to display "hello, world" is also been written.



