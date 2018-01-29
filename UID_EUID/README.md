# UID vs EUID

In this topic we are going to see the difference between uid and euid. We will also see a special attribute called setuid which will be discussed in the later part of the blog.

## UID
User ID, also called the real user ID. This corresponds to the real user ID of the user executing the program/process. This can only be set by super user and is usually set when the user logs in. Non-super user cannot change the user ID. 

## EUID
Effective User ID. This corresponds to the user ID the program/process is being executed as. The effective UID can be either set to read UID or set-UID. I will talk about the set-UID later in the article.

I know how confusing it can get. Lets use examples to get this clear. 

## Experiment 1
Lets take a simple program that prints UID(real user ID) and EUID(Effective UID). The function used are **getuid()** and **geteuid()**.
The program in the artice **uid_euid.c** simply prints the uid and euid. Lets run the program and see the results. 

```
$gcc uid_euid.c -o uid_euid.o
$./uid_euid.o
UID = 1000,EUID = 1000
```
The output above has the id 1000. This is the real user ID. This is the ID assigned to the user by the Operating System. This number could vary in you system however both the numbers have to be the same.
** So when program executes without any special permission the effective user ID is the same as real user ID**
Now lets try changing the UID and EUID.

## Experiment 2
In this example we will try to change the UID and EUID. The program **set_uid_euid.c** sets the UID and EUID using functions **setuid()** and **seteuid()**. The argument to the program are the user ID and effective UID. Now lets set the current userID of the user. In my case it is 1000. you can fid your from the previous program or execute ** id -u <username> ** where username is your username. 

```
$gcc set_uid_euid.c -o set_uid_euid.o
$./set_uid_euid.o 1000 1000
UID = 1000,EUID = 1000
```
We do not get any error. and the UID and EUID are printed as expected. Now lets try to set the UID and EUID to 0(root). Also we create a new user called **temp** using **useradd**. We try to change the UID and EUID to the ID of user **tempt**
```
$ sudo useradd temp
$ id -u temp
1001
$./set_uid_euid.o 0 0
Error User ID: Operation not permitted
Error Effective User ID: Operation not permitted
UID = 1000,EUID = 1000
$./set_uid_euid.o 1001 1001
Error User ID: Operation not permitted
Error Effective User ID: Operation not permitted
UID = 1000,EUID = 1000
```
IT throws a error. **So as a nornal(non-previledged) user we cannot change UID or EUID**. Lets try to execute the program as root.
```
$sudo ./set_uid_euid.o 0 0
UID = 0,EUID = 0
```
The UID ans EUID has changed successfully. **So root(previledged) user can change the UID and EUID of nay user to any number**.

Lets summarize ehat we learnt from the above experiments.
1. Only root(priveledged) user can change the UID or EUID to a number other than the current UID and EUID.
2. The EUID is the same as UID when the program is executed as normal. 

I have put a caveat in the second statement **when the program is execuated as normal** . So what is not normat and what will happen when that happens. This is where we come to **set-uid**.

## SET-UID
This is not a ID in general. This is a special bit set on programs that is set on programs called the set-user-ID bit. If this bit is set and we the used executes the program. The effective UID is set to the UID of the owner of the file. 
Again lets use examples to get this better. 

Before we go to programs, few file-systems have disabled the set-uid feature. So we need to create a new file system and mount and execute the programs there to see the effect of set-uid. 

Now lets create a small filesystem of 10MB. The steps to create a new file system is
```
$ dd if=/dev/zero of=small.fs bs=1024 count=10240
$ mkfs.ext3 small.fs
```

The dd command creates the space for the filesystem . The mkfs.ext creates create the ext3 file system on the small.fs.
Now lets mount the filesystem onto the subfolder sub. Then we move twos.txt file into that
```
$ mkdir sub
$ sudo mount small.fs sub/
$ sudo chown $USER sub
$ cd sub
```
The chown is required otherwise you will have to use sudo for each operation on the sub folder. 

## Experiemnt 3
Now lets run the same program **uid_euid.c** and **set_euid.c** and also with same parameters as before, but with a small change.
The change is we are going to set the **set-user-ID bit**. So copy both the programs into folder "sub". Before executing lets change the owner of the file and set the set-user-ID bit. 

```
$ gcc uid_euid.c -o uid_euid.o\
$ sudo chown temp uid_euid.o
$ sudo chmod +s uid_euid.o
```
The **chown** changes the owner of the file. So we change the owner of the file to **temp**. Next we set the set-user-ID. The **+s** in the chmod sets the set-used-ID bit on. We do the same for the other file(set_uid_euid.c). Now lets run the program **uid_euid.o** program.

```
$ ./uid_euid.o 
UID = 1000,EUID = 1001
```
What we notice here is the UID is the same as the ID of the user executing it. However EUID has now become **0**. So when a program executes which has the set-user-ID bit on. Then the effective UID becomes the ID of the owner of the file, which is the root here. **When set-user-ID bit is set then the program executes as the owner of the file rather as the real user**. Now lets run the other program. We try to set the UID and EUID to both 0(root) and 1001(temp).

```
$ ./set_uid_euid.o 0 0
Error User ID: Operation not permitted
Error Effective User ID: Operation not permitted
UID = 1000,EUID = 1001
$ ./set_uid_euid.o 1001 1001
UID = 1000,EUID = 1001
$ ./set_uid_euid.o 1000 1000
UID = 1000,EUID = 1000
```
As expected we get error when we set it to 0(root). However we did not get the error when set it to **1001**. However if you look at the output closely(**UID = 1000,EUID = 1001**). Only the EUID is changed. The UID is not changed. It did not throw an error but it ignored it. It did not throw the error becuase 1001 was the EUID of the program. **So only the root(previledged ) can change the user ID**. One more point to note is the third output(**UID = 1000,EUID = 1000**) when we set it to 1000(ID of current user). The EUID is changed to 1000. **The EUID can take either the value of UID or set-UID(if the bit is set)**. Now lets do another interesting experiment.
```
$ sudo chown temp uid_euid.o
$ sudo chmod +s uid_euid.o
$ ./uid_euid.o
UID = 1000,EUID = 0
$ ./set_uid_euid.o 0 0
UID = 0,EUID = 0
```
We changed the owner of the file to root. Now we execute the uid_euid.o which prints the UID and EUID. As expected the **UID is 1000** and **EUID is 0**. Now lets run the set_uid_euid.o by setting the value to 0(root). The output is **UID = 0,EUID = 0**. The UID changed to 0, which should not. Hope you were able to reason it out. Had it beed any other user the value would have been ignored. Since the owner of the program was roor. The EUID is 0. So it runs as root(previledged) user. So it has the capacity to chage the user ID.


Now lets summerize all the learnings
1. The UID can only be changed by previledged users.
2. The EUID can either take the value of UID or set-UID(if the bit is set).

Hope now the difference between UID and EUID is clear. 

