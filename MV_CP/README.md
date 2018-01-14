# mv vs cp

In EXT file system each file has a inode number. You will be able to get the inode number by 

```
ls -l
```
## mv
When we move a file the inode is removed from the existing directory and mapped to the new directory. The inode number of the file is not changed. Except in few cases which we will dicuss later

## cp 
When we copy a file a new file is created and the contents will be copied to the file. So the existing file will have the same inode number the new file will have different inode number.

Lets see how this works with programs as it will give a better understanding as it did for me.

## Programs
We have two programs **test_write.c** and **test_mmap.c**. 

The **test_write.c** file creates a file of size ~6MB. The arguments to the file are the name of the file and a number. What the program does is it creates a file filling it with the number

```
$gcc test_write.c -o test_write
$./test_write ones.txt 1
```
The above program creates a file filled with 1. Similarly to create a file called twos.txt filled with 2.
```
$./test_write twos.txt 2
```

The **test_mmap.c** file mmaps the entire region of the file given as input and it will randomly take a   The reason we chose mmap is because mmap directly maps the file to page tables. So it reflects the contents of the page immediately when changed. We can test what happend to the underlying file when moved or copied. The inputs to this file are

```
$gcc test_mmap.c -o test_mmap
$./test_mmap ones.txt
```

What we do in the program is randomly index in to the file and print the number.

## Experiment 1
Now lets jump into the experiment. create a two files ones.txt and twos.txt with 1 and 2 filed respectively. keep the ones.txt in the curent folder and mv the twos.txt file into a subfolder. I have put put down the steps to do the same. 

```
$./test_write ones.txt 1
$./test_write twos.txt 2
$mkdir sub
$mv twos.txt sub/
```

The directory heirechy will look like this 
```
.
├── ones.txt
├── sub
│   └── twos.txt
├── test_mmap.c
├── test_write
└── test_write.c
```

Now lets see their inode numbers. The inode numbers will mostly be different in your system.
```
$ ls -i ones.txt sub/twos.txt 
16777708 ones.txt  16777712 sub/twos.txt
```

Now execute the **test_mmap** with the file ones.txt as input
```
$./test_mmap ones.txt 
```
You will see the program continously print the values form ramdom position of the file. (It values will all be 1). The printed format is below
```
num[589622] = 1
num[20628] = 1
num[126064] = 1
num[41397] = 1
```
The number between the [] are the index position of the file. 

***
### Now lets move the file as the same time the program is running
***
```
$ mv sub/twos.txt ones.txt
```
We are moving the file twos on the directory as ones.txt. So the program should start printing twos now. Does it?
### NO
Now lets see the inode numbers.  

```
ls -i ones.txt 
16777712 ones.txt
```

The inode number for the file did not change when we moved froms twos.txt to ones.txt. So old ones.txt file has the same file and it is still in the file system. It will not be deleted till the program stops. There are ways to print the filename and content using **debugfs**. I will cover it in a different chapter. 

If you stop the program and run again, you will see that the program now prints 2.

## Experiment 2
Now lets check how cp works. Let get back the same test environment as Experiment one. 
```
$ tree .
.
├── ones.txt
├── sub
│   └── twos.txt
├── test_mmap
├── test_mmap.c
├── test_write
└── test_write.c

1 directory, 6 files
$ ls -i ones.txt sub/twos.txt 
16777708 ones.txt  16777712 sub/twos.txt
```

As before run the test_mmap program with ones.txt. As it prints 1s, copy the twos.txt in to the folder as ones.txt.
```
cp sub/twos.txt ones.txt
```

Now we see the program immediately start printg 2s. Let us see the inode number of the files.
```
$ ls -i ones.txt sub/twos.txt 
16777708 ones.txt  16777712 sub/twos.txt
```
The old ones.txt inode number is retained it is not changed to the twos.txt. So there we go

## Experiment 3
In this experiment lets have a similer test environment as both experiments as Experiment one and two, except the sub folder will be a different file system. We will create a new file system and mount is to the sub folder. Dont worry I will explain the commands and explain them. As before create two files ones.txt and twos.txt. and create a subfolder sub, but do not move the twos into sub folder.

```
.
├── ones.txt
├── sub
├── test_mmap
├── test_mmap.c
├── test_write
├── test_write.c
└── twos.txt

```
Now lets create a small filesystem of 10MB
```
$ dd if=/dev/zero of=small.fs bs=1024 count=10240
$ mkfs.ext3 small.fs
```

The dd command creates the space for the filesystem . The mkfs.ext creates create the exxt3 file system on the small.fs.
Now lets mount the filesystem onto the subfolder sub. Then we move twos.txt file into that
```
$ sudo mount small.fs sub/
$ sudo chown $USER sub
$ mv twos.txt sub/
```
The chown is required otherwise you will have to use sudo for each operation on the sub folder. The folder heirachy will look like this. 

```
.
├── ones.txt
├── small.fs
├── sub
│   └── twos.txt
├── test_mmap
├── test_mmap.c
├── test_write
└── test_write.c
```

It looks very same, but remember that sub is file system. Lets look at the inode number.

```
$ ls -i ones.txt sub/twos.txt 
16777712 ones.txt        12 sub/twos.txt
```
I think you have guessed why the inode number for twos.txt is small. It is beacuse it is a new file system and there are no files in the file system(there could be hidden file system related files).

Now lets do the experiment as in Experiment 1. execute the **test_mmap** file with ones.txt

```
$ ./test_mmap ones.txt
```
While the program execute mv the file twos.txt into the folder as ones.txt. Lets see what happens.

```
$ mv sub/twos.txt ones.txt 
```
IT still continues to print 1s, but if you look at the inode numbers.

```
$ ls -i ones.txt 
16777708 ones.txt

```
where did this inode come from? it is not supposed to be 12 as per experiment 1? That is what happens when we mv between filesystems. It cannot do a direct mapping of inode number becuase the inode 12 is only for the small.fs file system. The same 12 could poin to some other file in out host filesystem . So the system does a copy to the file system and then maps the copies file inode number to the directory. 

### Summary
 When we copy a file, the contents inside the old file is written with the content from the new file. The inode number of the old file is the same. Whereas when we move the file the inode of the moved file will be mapped to the directory. No copying is done. When we move between file systems. First a copy of file is craeted in the other file system and then the inode number of the new file is mapped to the directory.
