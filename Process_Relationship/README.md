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
The PGID is he same as the PPID of the child which is the same as PID as the Parent. We can move process between groups or create a new process group using the **setpgid()** function. In the next experiment lets play with this function. 

### Experiment 5
Let's run the program **proc_pgid.c**. This program has the parent process which created two child process. The child should have its PGID same as PID of the parent(as seen in Experiment 4). What we do in the program is we create a new group for child1. Since it is the only child its PID is the PGID. Now we try to move child2 from parent group to the newly created child1 group.

```
$ gcc proc_pgig.c -o proc_pgig.o
$ ./proc_pgig.o 
Parent:
	PID = 30543
	PPID = 28165
	PGID = 30543
Creating a new group for child1
Child1:
	PID = 30544
	PPID = 30543
	PGID = 30544
Adding child2 to newly created group
Child2:
	PID = 30545
	PPID = 30543
	PGID = 30543
```
In the program we call fork() which creates new child1 process. Then the child1 process calls **setpgid(getpid(),0)**. which should create the a new group since the second paramenter which is the group ID is zero. Then we call fork() again to create child2. After child2 is created the child2 calls **setpgid(getpid(),pid_child1)** which should moves child2 to child1 group.  

In the output above the Child1 PGID is the same as Child1 PID. Also the Child2 PGID is not the Child1 PID as expected. It is the PID of the Parent. This shows **"A process cannot associate itself with any group it wants to"**. However the PPID of both the childs are the parent PID. Now lets modify this expriment a little. Instead of the child moving into different groups. Lets have the parent move the childs to different groups.

### Experiment 6
Lets run **proc_pgid1.c** which does the same as **proc_pgid.c** except that the group changing is done by parent. 
```
$ ./proc_pgig1.o 
Parent:
	PID = 31008
	PPID = 28165
	PGID = 31008
Creating a new group for child1
Child1:
	PID = 31009
	PPID = 31008
	PGID = 31009
Adding child2 to newly created group
Child2:
	PID = 31010
	PPID = 31008
	PGID = 31009
```
Now the child2 PGID is the same as child1 PID. This shows **A parent can change its childrens group"**.

## SID
This is the session ID. Like the other IDs, session ID is not a real ID. Its is just the ID of the process whose the session leader. A session is a group of process groups. Lets start with the experiment to understand this. 

### Experiment 7
Lets run the **proc_sid.c** program . This program just creates two children and each process prints the PID, PPID, PGID and SID. 
This program will block since I want to demonstrate few things. So we need to execute it as a background process. 
```
$ gcc proc_sid.c -o proc_sid.o
$ ./proc_sid.o & 
Child1:
	PID = 31870
	PPID = 31869
	PGID = 31869
	SID = 28165
Child2:
	PID = 31871
	PPID = 31869
	PGID = 31869
	SID = 28165
Parent:
	PID = 31869
	PPID = 28165
	PGID = 31869
	SID = 28165
```

Now lets us the ps command to analize the processes and their relation ship.
```
$ ps -o pid,ppid,pgid,sid,comm
  PID  PPID  PGID   SID COMMAND
28165  2942 28165 28165 bash
28450 28165 28450 28165 emacs
31945 28165 31945 28165 proc_sid.o
31946 31945 31945 28165 proc_sid.o
31947 31945 31945 28165 proc_sid.o
31951 28165 31951 28165 ps
```
Looking as the output above. We have three processes of **proc_sid**(one parent and two child). Along with that I also have emacs which i have used for coding. Then we have the terminal process. We have 6 processs with unique PIDs. We have 4 PGIDs. All the process we created is the same group with ID 31945 which is the PID of our parent process. The emacs has a seperate PGID(28450) which is the same as the PID(28450) of emacs, since it is the only process in the group. Similarly for ps(31951) and bash(28165). Looking at PPID(31945) of the child processes we created has the PPID as out parent PID(31945). All the other process have bash PID(28165) as PPID. Now lets look at the SID. All processes have the SID as 28165. This is the same as PGID and PID of bash. 

** All session ID are a process group ID but eh vice versa is not the same**

So session is a group of process groups of which one of the PGID is the session ID. 

To terminate the process run **fg** and press enter thrice. 

