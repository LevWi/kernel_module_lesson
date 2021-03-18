#include <linux/init.h>  
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>    

#define  DEVICE_NAME "fifo"   
#define  CLASS_NAME  "fif"    

MODULE_LICENSE("GPL");    
MODULE_AUTHOR("Lev Fomenko"); 
MODULE_DESCRIPTION("A simple FIFO device"); 
MODULE_VERSION("0.1");           

static int    majorNumber;              
static struct class*  fifoClass  = NULL;
static struct device* fifoDevice = NULL;


static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int __init fifo_init(void){
   printk(KERN_INFO "FIFODev: init\n");

   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      return majorNumber;
   }

   // Register the device class
   fifoClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(fifoClass)){                
      unregister_chrdev(majorNumber, DEVICE_NAME);
      return PTR_ERR(fifoClass);          
   }

   // Register the device driver
   fifoDevice = device_create(fifoClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(fifoDevice)){               
      class_destroy(fifoClass);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(fifoDevice);
   }
   return 0;
}

static void __exit fifo_exit(void){
   device_destroy(fifoClass, MKDEV(majorNumber, 0));     
   class_unregister(fifoClass);                          
   class_destroy(fifoClass);                             
   unregister_chrdev(majorNumber, DEVICE_NAME);          
   printk(KERN_INFO "FIFODev: fifo_exit\n");
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

module_init(fifo_init);
module_exit(fifo_exit);