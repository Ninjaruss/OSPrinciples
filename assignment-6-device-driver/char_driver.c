/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Russell Azucenas
* Student ID: 917422693
* Project: Assignment 6 – Device Driver
*
* File: char_driver.c
*
* Description: Keeps a buffer for a char string.
*
**************************************************************/
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>        // Required for the copy to user function
#include <linux/slab.h>           // For kmalloc and kfree

#define  DEVICE_NAME "char_driver"    ///< The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "char"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Russell Azucenas");
MODULE_DESCRIPTION("CSC415 char_driver module");
MODULE_VERSION("0.1");

#define BUFFER_SIZE 1024

// Driver's 'global' buffer
static char * charBuffer;

static int deviceNumber;
static int openCount = 0;
static int closeCount = 0;
static int errorCount = 0;

static int     cd_open(struct inode *, struct file *);
static int     cd_release(struct inode *, struct file *);
static ssize_t cd_read(struct file *, char *, size_t, loff_t *);
static ssize_t cd_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = 
{
	.open	 = cd_open,
	.release = cd_release,
	.read	 = cd_read,
	.write = cd_write,
};

// Runs once the system calls the driver
static int __init cd_init(void)
{
	printk(KERN_INFO "[Initializing char_driver]\n");
   printk(KERN_ALERT "Registering device number...\n");

   // Handle device registration for use in system
   deviceNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (deviceNumber < 0)
   {
      printk(KERN_ALERT "Failed to register device number!\n");
      return deviceNumber;
   }
   printk(KERN_INFO "Successfully registered device number! (deviceNumber: %d)\n", deviceNumber);

   // Allocation of space for the module's buffer
   printk(KERN_INFO "Allocating space for char buffer...\n");
   charBuffer = (char*) kmalloc(BUFFER_SIZE, GFP_KERNEL);
   if (!charBuffer)
   {
      printk(KERN_ALERT "Failed to allocate space for char buffer!\n");
      return -1;
   }
   printk(KERN_INFO "Successfully allocated space for char buffer.\n");
	return 0;
}

// Upon exiting the driver, free the buffer and delete driver from driver list
static void __exit cd_exit(void)
{
	printk(KERN_INFO "Exiting char_driver. Initiating clean up.\n");
	kfree(charBuffer);
	unregister_chrdev(deviceNumber, DEVICE_NAME);
   printk(KERN_INFO "Finished clean up.\n");
}

// Reads the driver's buffer into received buffer
ssize_t cd_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
   // Adjust length if trying to read more characters than available
   if (len > BUFFER_SIZE - *offset)
   {
      len = BUFFER_SIZE - *offset;
   }

   errorCount = 0;
	// Copies user's buffer to driver buffer (uaccess.h)
   errorCount = copy_to_user(buffer, charBuffer, len);
 
   if (errorCount == 0)
   {
      printk(KERN_INFO "char_driver: Sent %d characters to the user\n", (int)len);
      // Move offset by length of received buffer
      *offset += len;
      return len;
   }
   else 
   {
      printk(KERN_INFO "char_driver: Failed to send %d characters to the user\n", errorCount);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

// Writes the received buffer into driver's buffer
ssize_t cd_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
   copy_from_user(charBuffer + *offset, buffer, len);
	// Moves the offset since we have written the characters from received buffer
   *offset += len;

   printk(KERN_INFO "char_driver: Wrote %d characters to buffer.\n", (int)len);
   return len;
}

// Occurs when the system opens the device
int cd_open(struct inode *inodep, struct file *filep)
{
   openCount++;
	printk(KERN_INFO "char_driver: Opened %d time(s)\n", openCount);
	return 0;
}

// Occurs when the system closes the device
int cd_release(struct inode *inodep, struct file *filep)
{
   closeCount++;
	printk(KERN_INFO "char_driver: Closed %d time(s)\n", closeCount);
	return 0;
}

module_init(cd_init);
module_exit(cd_exit);
