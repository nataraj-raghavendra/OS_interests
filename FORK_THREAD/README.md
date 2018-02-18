# Fork vs Thread
Before we understand the difference beween a fork and thread we need to understand what is a process and elements of process. 

## Process
A process is a program in execution. When a operating system executes a program it is called a process. So we may have a sigle program executed simultaneously multiple times which creates multiple processes. Also we may have a single program which creates multiple processes in the program. 
Each process has a unique ID called the process ID allocated by OS. 

## Elements of a process
Each process is made up of few sections. 
1. **text**:
  This is where the code of the process reside. This is a readonly section.
2. **data**:
  This is where global and static initialized variables  are stored.
3. **rodata**:
  This is where constant variables  are stored.
3. **bss**:
  This is where global and static un-initialized variables  are stored.
4. **stack**:
  This is where the local variables and function calls are stored.
5. **heap**:
  This is where user allocated memory is stored. When the user uses malloc, calloc, new etc.
  
Let do a small experiment to show the different elements of process. 

### Experiment 1:
Lets compile the program and run the commands below. The output displayed below is a truncated version. I have only displayed the interesting parts of the output.  

```
$ gcc elements_process.c -o elements_process.o
$ objdump -t elements_process.o | grep .rodata
00000000004005e0 l    d  .rodata	0000000000000000              .rodata
00000000004005e0 g     O .rodata	0000000000000004              _IO_stdin_used
00000000004005e4 g     O .rodata	0000000000000004              constant

$ objdump -t elements_process.o | grep .data
0000000000601028 l    d  .data	0000000000000000              .data
000000000060103c l     O .data	0000000000000004              init_stk
0000000000601038 g     O .data	0000000000000004              init

$ objdump -t elements_process.o | grep .bss
0000000000601040 l    d  .bss	0000000000000000              .bss
0000000000601044 l     O .bss	0000000000000004              un_init_stk
0000000000601048 g     O .bss	0000000000000008              gl_heap
0000000000601050 g     O .bss	0000000000000004              un_init

```
As you see the variables init_stk and init are in the data section. The un_init_stk,un_init are in the .bss section. The constant is in rodata section. You may be wondering where is my local variables. The will be allocated memory in the program. to give you a hint i have showd the assembly version of the main function . In this the line **40052a:	48 83 ec 10          	sub    $0x10,%rsp** is where the local variables are. I leave this here. 

```
0000000000400526 <main>:
  400526:	55                   	push   %rbp
  400527:	48 89 e5             	mov    %rsp,%rbp
  40052a:	48 83 ec 10          	sub    $0x10,%rsp
  40052e:	c7 45 f0 14 00 00 00 	movl   $0x14,-0x10(%rbp)
  400535:	c7 45 f4 1e 00 00 00 	movl   $0x1e,-0xc(%rbp)
  40053c:	bf 01 00 00 00       	mov    $0x1,%edi
  400541:	e8 ca fe ff ff       	callq  400410 <malloc@plt>
  400546:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
  40054a:	bf 01 00 00 00       	mov    $0x1,%edi
  40054f:	e8 bc fe ff ff       	callq  400410 <malloc@plt>
  400554:	48 89 05 ed 0a 20 00 	mov    %rax,0x200aed(%rip)        # 601048 <gl_heap>
  40055b:	90                   	nop
  40055c:	c9                   	leaveq 
  40055d:	c3                   	retq   
  40055e:	66 90                	xchg   %ax,%ax

```

Now lets get to the crux of the articale FORK vs THREAD.

## FORK
Forking is nothing but creating a new process. We create a new process that copies all the elements of old process.

## THREAD
Threading is a light weight process which shares all the section of the process except for the stack. A process can have multiple threads. 

Lets understand this by a experiment

### Experiment 2:
Lets execute program forking.c. We have variables declared as global(init & unint), heap and local to function variable. We create another process using fork. The child process increments the variables. Then both child and parent print the variables value.  
```
$ gcc forking.c -o forking.o
$ ./forking.o 
Process : 3317 => init = 6,un_init=6,local=6,int_heap=6
Process : 3316 => init = 5,un_init=5,local=7,int_heap=5
```
As you see none of the values printed by parent is affected by child. Now lets see the threading experiment.

### Experiment 3:
Lets execute program threading.c. We have variables declared as global(init & unint), heap and local to function variable. We create another thread. The created thread increments the variables. Then both created thread and main thread print the variables value.  

```
$ gcc forking.c -o forking.o
$ ./forking.o 
Process : 3317 => init = 6,un_init=6,local=6,int_heap=6
Process : 3316 => init = 5,un_init=5,local=7,int_heap=5
```
As you see all the vlaues except the local variable are affected but created process incrementing variable. The local varibale is stored in stack. So this shows that thread have their own stack. The other sections are shred by threads.
