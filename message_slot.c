#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "message_slot.h"

MODULE_LICENSE("GPL");

// TODO: The censorship state should be maintained per file descriptor (you can store it in file->private_data along with the channel id).
static bool should_encrypt = false;

// Device setup
struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
    // .release = device_release,
};

static int __init message_slot_init(void)
{
    int major = register_chrdev(MSG_SLOT_DEVICE_MAJOR, MSG_SLOT_DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Registering %s char device failed with %d\n", MSG_SLOT_DEVICE_NAME, major);
        return major;
    }
    printk(KERN_INFO "%s device registered with major number %d\n", MSG_SLOT_DEVICE_NAME, major);
    return GENERAL_SUCCESS;
}

static void __exit message_slot_exit(void)
{
    unregister_chrdev(MSG_SLOT_DEVICE_MAJOR, MSG_SLOT_DEVICE_NAME);
}

module_init(message_slot_init);
module_exit(message_slot_exit);

// Device functions

// static int device_release(struct inode* inode, struct file* file)
// {
//     return GENERAL_SUCCESS;
// }

static int device_open(struct inode* inode, struct file* file)
{
    return GENERAL_SUCCESS;
}

static ssize_t device_read(struct file* file, char __user* buffer, size_t length, loff_t* offset)
{
    return 0;
}

static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset)
{
    return 0;
}

static long device_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{
    return 0;
}
