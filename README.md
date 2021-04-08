# Fifo charter device example

It is simple fifo buffer for byte data

Installation LKM:
```
$ sudo insmod /path/to/fifo.ko
$ dmesg
...
[14253.829416] FIFODev : alloc_chrdev_region successful
[14253.829419] FIFODev : major number of our device is 238
[14253.829421] FIFODev : to use mknod /dev/fifo c 238 0    
[14253.829424] FIFODev : device addition to the kernel successful

$ sudo mknod /dev/fifo c 238 0
$ sudo chmod a+rw /dev/fifo
```
Test:
```
$ echo "TEST1" > /dev/fifo
$ echo "TEST2" > /dev/fifo
$ echo "TEST3" > /dev/fifo
$ cat /dev/fifo
TEST1
TEST2
TEST3
```

Remove LKM from system:
```
$ sudo rmmod fifo.ko
```

