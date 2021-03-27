#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#include "string_buffer.h"

#define  DEVICE_NAME "fifo"

static int g_major_number;
static struct cdev g_cdev;
static dev_t g_dev_num;


DEFINE_MUTEX(g_buffer_mtx);
static struct string_buffer g_string_buffer;
static struct kmem_cache_t * g_cache;
//TODO +slab_cache +string_buffer

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations g_fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static struct substring* substring_slab_alloc(void* allocator) {
   return kmem_cache_alloc(&g_cache, GFP_KERNEL);
}

substring_slab_free()

static int __init fifo_init(void) {

   g_cache = KMEM_CACHE(substring, 0);
   if (!g_cache) {
      return -ENOMEM;
   }

   string_buffer_init(&g_string_buffer);
   
   //TODO init destructor / constructor

   int ret = alloc_chrdev_region(&g_dev_num, 0, 1, DEVICE_NAME);
   if (ret < 0) {
      printk(KERN_ERR " FIFODev : alloc_chrdev_region failed\n");
      return ret;
   } else {
      printk(KERN_INFO " FIFODev : alloc_chrdev_region successful\n");
   }

   g_major_number = MAJOR(g_dev_num);
   printk(KERN_INFO "FIFODev : major number of our device is %d\n", g_major_number);
   printk(KERN_INFO "FIFODev : to use mknod /dev/%s c %d 0\n", DEVICE_NAME, g_major_number);

   cdev_init(&g_cdev, g_fops);
   g_cdev.owner = THIS_MODULE

   ret = cdev_add(g_cdev, g_dev_num, 1);
   if (ret < 0) {
           printk(KERN_ERR "FIFODev : device adding to the kernel failed\n");
           return ret;
   } else {
           printk(KERN_INFO "FIFODev : device addition to the kernel successful\n");
   }

   return 0;
}

static void __exit fifo_exit(void){
   cdev_del(&g_cdev); /*removing the structure that we added previously*/
   printk(KERN_INFO " FIFODev : removed the cdev from kernel\n");

   unregister_chrdev_region(g_dev_num, 1);
   printk(KERN_INFO  " FIFODev : unregistered the device numbers\n");
   printk(KERN_ALERT " FIFODev : character driver is exiting\n");
}

static int dev_open(struct inode *inodep, struct file *filep){

   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   return -EFAULT;
}


static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
   return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lev Fomenko");
MODULE_DESCRIPTION("A simple FIFO device");
MODULE_VERSION("0.1");

module_init(fifo_init);
module_exit(fifo_exit);
