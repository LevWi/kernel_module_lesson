#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "string_buffer.h"

#define  DEVICE_NAME "fifo"

static int g_major_number;
static struct cdev g_cdev;
static dev_t g_dev_num;


DEFINE_MUTEX(g_buffer_mtx);
static struct string_buffer g_string_buffer;
static struct kmem_cache * g_cache;

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
   struct substring* tmp = (struct substring*)kmem_cache_alloc(g_cache, GFP_KERNEL);
   if (tmp) {
      printk(KERN_DEBUG " FIFODev : [OK] substring_slab_alloc\n");
      substring_init(tmp);
   } else {
      printk(KERN_WARNING " FIFODev : [NOK] substring_slab_alloc\n");
   }
   return tmp;
}

static void substring_slab_free(void* allocator, struct substring* ss) {
   printk(KERN_DEBUG " FIFODev : substring_slab_free\n");
   kmem_cache_free(g_cache, ss);
   return;
}

static int __init fifo_init(void) {

   g_cache = KMEM_CACHE(substring, 0);
   if (!g_cache) {
      return -ENOMEM;
   }

   string_buffer_init(&g_string_buffer);
   g_string_buffer.substring_new = substring_slab_alloc;
   g_string_buffer.substring_free = substring_slab_free;

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

   cdev_init(&g_cdev, &g_fops);
   g_cdev.owner = THIS_MODULE;

   ret = cdev_add(&g_cdev, g_dev_num, 1);
   if (ret < 0) {
      printk(KERN_ERR "FIFODev : device adding to the kernel failed\n");
      return ret;
   } else {
      printk(KERN_INFO "FIFODev : device addition to the kernel successful\n");
   }

   return 0;
}

static void __exit fifo_exit(void){
   cdev_del(&g_cdev);
   printk(KERN_INFO " FIFODev : removed the cdev from kernel\n");

   unregister_chrdev_region(g_dev_num, 1);

   // TODO is locking needed here?
   mutex_lock(&g_buffer_mtx);
   string_buffer_clear(&g_string_buffer);
   mutex_unlock(&g_buffer_mtx);

   kmem_cache_destroy(g_cache);

   printk(KERN_INFO  " FIFODev : unregistered the device numbers\n");
   printk(KERN_ALERT " FIFODev : character driver is exiting\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   printk(KERN_INFO  " FIFODev : open\n");
   return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO  " FIFODev : release\n");
   return 0;
}

// Return zero for success
static int copy_to_user_callback(char *to, const char *from, size_t count, void* context) {
   return copy_to_user(to, from, count);
}

// Return zero for success
static int copy_from_user_callback(char *to, const char *from, size_t count, void* context) {
   return copy_from_user(to, from, count);
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   printk(KERN_INFO  " FIFODev : read\n");

   mutex_lock(&g_buffer_mtx);

   ssize_t result = string_buffer_extract(&g_string_buffer, buffer, len, copy_to_user_callback, NULL);
   //TODO *f_pos += count; is it needed?

   mutex_unlock(&g_buffer_mtx);
   return result < 0 ? -EFAULT : ((ssize_t)len - result);
}


static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   printk(KERN_INFO  " FIFODev : write\n");

   mutex_lock(&g_buffer_mtx);

   ssize_t result = string_buffer_append(&g_string_buffer, buffer, len, copy_from_user_callback, NULL);
   //TODO *f_pos += count; is it needed?

   mutex_unlock(&g_buffer_mtx);
   return result < 0 ? -EFAULT : ((ssize_t)len - result);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lev Fomenko");
MODULE_DESCRIPTION("A simple FIFO device");
MODULE_VERSION("0.1");

module_init(fifo_init);
module_exit(fifo_exit);
