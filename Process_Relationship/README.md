# Process Relationship

In this article we will see how the processes are related in linux operating system. This article assumes the reader has knowledge about process and how to creata a process programatically.

Lets get started with different IDs related to process.
##PID
  Each process has a unique ID called the process ID. This number is unique for each process. **getpid()** function gets the PID of the current process. 
  
##PPID
  It is the parent process ID. It is the ID of the process that created the current process. **getppid** function gets the parent PID of the current process. 
  
Before we get to other IDs lets understand PID and PPID through few experiments. 

### Experiment 1
Lets run the program **proc_det.c**. This program just prints the PID and PPID. 
```
$ gcc proc_det.c -o proc_det.o
$ ./proc_det.o 
PID = 29185
PPID = 28165
```
The process ID is 29185 and the parent PID is 28165. Who is the parent of this process? It is the shell. We can use the ps command to verify that.
```
$ ps
  PID TTY          TIME CMD
28165 pts/2    00:00:00 bash
28450 pts/2    00:00:03 emacs
29229 pts/2    00:00:00 ps
```
The bash has the PID 28165 which is the same as the PPID of our program. Now lets become parent process by creating child process using fork. 

### Experiment 2
Lets run the program **proc_child.c**. This program forks a child process and both parent and child prints the PID and PPID. 
```
$ gcc proc_child.c -o proc_child.o
$ ./proc_child.o 
Parent:
	PID = 29374
	PPID = 28165
Child:
	PID = 29375
	PPID = 29374
```
The child PPID is 29374 which is the same as the PID of our parent process. The the process which created the process becomes the process. Similarly the PPID of main(parent) process is 28165 which is our bash PID.
```
$ ps
  PID TTY          TIME CMD
28165 pts/2    00:00:00 bash
28450 pts/2    00:00:08 emacs
29402 pts/2    00:00:00 ps
```

Ok now what happens if our parent exits before our child exits. Lets try that. 

### Experiment 3
Lets run the program **proc_parent_exit.c**. This program makes the parent exit before the child completes the execution. 
```
$ gcc proc_parent_exit.c -o proc_parent_exit.o
$ ./proc_parent_exit.o 
Parent:
	PID = 29468
	PPID = 28165
$ Child:
	PID = 29469
	PPID = 1811
```
**The program may print the child contents in a wierd way. You may recieve the terminal promt before the child prints the contents. This is due to the main process exiting.**

Looking in to the output, the child's PPID is 1811(not our parent process). 
```
$ ps -e | grep 1811
 1811 ?        00:00:00 upstart
```
How did this process become a parent to the process we create. When the parent dies before the child process. The child get associated with upstart as parent. This is the process  that cleans up orphans process. This can vary based on different operating systems. This is related to life cycle of the process. I will create a seperate article for the process lifecycle. 

## PGID
  This is the process Group ID. It is the PID of the process group leader. We can have a group of process that are created to achieve a purpose. Let's understand it with a experiment.
  
### Experiment 4
Let's execute the program **proc_child1.c**. This is the same program as **proc_child.c** excepts it prints the PGID using the function **getpgrp()** or **getpgid(0)**. The zero here will fetch the PGID of the executing process. 
```
$ gcc proc_child1.c -o proc_child1.o
$ ./proc_child1.o 
Parent:
	PID = 30146
	PPID = 28165
	PGID=30146
Child:
	PID = 30147
	PPID = 30146
	PGID=30146
```
The PGID is he same as the PPID of thec child which is the sme as PID as the Parent. We can also create

