# Process Relationship

In this article we will see how the processes are related in linux operating system. This article assumes the reader has knowledge about process and how to creata a process programatically.

Lets get started with different IDs related to process.
## PID
  Each process has a unique ID called the process ID. This number is unique for each process. **getpid()** function gets the PID of the current process. 
  
## PPID
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

**All session ID are a process group ID but the vice versa is not the same**

So session is a group of process groups of which one of the PGID is the session ID. 

To terminate the process run **fg** and press enter thrice.

Now lets try changing the session ID. 

### Experiment 8
We will change the session ID of Child1 and the parent. Let's run the program **proc_setsid.c**. This frogram is the same as **proc_sid.c**(Experiment 7). It additionally calls **setsid()** which creates a new session and also prints the return value of the setsid function.

```
./proc_setsid.o 
Createing new session Child1. Return value = 32358
Child1:
	PID = 32358
	PPID = 32357
	PGID = 32358
	SID = 32358
Child2:
	PID = 32359
	PPID = 32357
	PGID = 32357
	SID = 28165
Createing new session parent. Return value = -1
Parent:
	PID = 32357
	PPID = 28165
	PGID = 32357
	SID = 28165
```
The Child1 new SID is the smae as PID and PGID of Child1. The session 32358(child1) has a single process whose PID is the same.
If you notice the setsid fails in case of parent and return the new SID incase of child. **setsid() will return error if the process is already a process group leader.** So process group leaders cannot create new sessions. 

##Controlling Terminal
Each session has a single controlling terminal. The process groups in a session can be divided to foreground and background  process groups. Each session has one foreground process group and many background session groups. Any termainal signals lize SIGINT and SIGSTOP are delivered to the foreground processs group. Lets understand this  concept using experiments. 

### Experiment 9
Lets execute the program **proc_tcgetpgrp.c**. This program prints the PID,SID,PGRP,TC_PGRP(the controlling terminal process).
The function **tcgetpgrp(int fd)** returns the process group ID of the foreground process group associated with the terminal open in fd. In the program we use STDIN_FILENO which is the stdin of the terminal. 

```
$ gcc proc_tcgetpgrp.c -o proc_tcgetpgrp.o
$ ./proc_tcgetpgrp.o 
PID=>7327
SID=>4764
PGRP=>7327
TC_PGRP=>7327
```
Here there is only one process(7327) so that is a leader of the process group and also the controlling process group ID. So the main process is the controlling process group with only one process. Now lets try sending signal to the process to verify it is the forground process.

### Experiment 10
Lets execute **proc_tcgetgrp_signal.c** program. This program registers a signal function for SIGINT. So when the signal SIGINT(Signal Interrupt) the function registered(signal_fn) is called. The SIGINT in terminal can be sent by pressing **ctrl + c** in the terminal. I have included a sleep of 5 seconds for time to send the SIGINT signal. 
```
$ gcc proc_tcgetpgrp_signal.c -o proc_tcgetpgrp_signal.o
ragha:Process_Relationship$ ./proc_tcgetpgrp_signal.o 
PID=>7479
SID=>4764
PGRP=>7479
TC_PGRP=>7479
^C
Received Signal to main process 2
```
The signal is sent to the controlling process group(foreground process group) and the registered signal function is called.
Now lets one more process to the foreground process group and registering another function to SIGINT signal. 

### Experiment 11
Lets execute the program **proc_tcgetpgrp_siggrp.c**. This program creates two process and each process registers to different signal function.  

```
$ gcc proc_tcgetpgrp_siggrp.c -o proc_tcgetpgrp_siggrp.o
$ ./proc_tcgetpgrp_siggrp.o 
Parent:PID=>7618
SID=>4764
PGRP=>7618
TC_PGRP=>7618

Child:PID=>7619
SID=>4764
PGRP=>7618
TC_PGRP=>7618
^C
Received Signal to main process 2
Received Signal to child process 2
```

The above output shows the foreground process group(7618) which has two process(7818 & 7819). Each of the process in this group(parent and child) registers for a seperate signal functions. So when we send SIGINT by pressing **ctrl+c** the signal is sent to all processes in the foreground process group. So both process in the program(parent and child) receive the signal and we see both the print statements outputs in the program. Now lets move the child process to a seperate group.

### Experiment 12
This program is the same as previous(Experiment 11) except we call the setpgid function in child process. As discussed earlier the function assigns the process to a process group. 
```
$ gcc proc_tcgetpgrp_setsid.c -o proc_tcgetpgrp_setsid.o 
$ ./proc_tcgetpgrp_setsid.o 
Parent:PID=>8002
SID=>4764
PGRP=>8002
TC_PGRP=>8002

Child:PID=>8003
SID=>4764
PGRP=>8003
TC_PGRP=>8002

^CReceived Signal to main process 2
```
The output above shows that only the main process receives the signal SIGINT and the child process does not receive the signal. The reason is when we create a new group the group goes as a background process group becuase a session can have only one foreground process group. Now lets make the child take control of the terminal. 

### Experiment 13
Lets execute the **proc_tcsetpgrp.c**. This is the same as the previous experiment except we make the child process the foreground process group. We use the **tcsetpgrp** function to set the foreground process group.  
```
$ gcc proc_tcsetpgrp.c -o proc_tcsetpgrp.o 
$ ./proc_tcsetpgrp.o 
Parent:PID=>8166
SID=>4764
PGRP=>8166
TC_PGRP=>8166

Parent:PID=>8166
SID=>4764
PGRP=>8166
TC_PGRP=>8167

Child:PID=>8167
SID=>4764
PGRP=>8167
TC_PGRP=>8167

^C
Received Signal to child process 2
```
The output above shows that the parent initially is the foreground process group(8166). After calling the **tcsetpgrp** the child is the foreground process group(8167). So the  second print for parent shows that TC_PGRP changed to 8167. When we send the SIGINT signal the parent no longer receives the signal. The child receives the signal because the process becomes the foreground process group and the parent becomes the background process group. So the singal_fn_child is called.

I hope this article helped in understanding the relationships between processes, session and terminal. 
