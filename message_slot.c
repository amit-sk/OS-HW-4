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
#include <linux/string.h>

#include "message_slot.h"

MODULE_LICENSE("GPL");


// For each message slot file, assume that no more than 2^20 message channels will be used.
typedef struct message_slot_channel {
    unsigned long channel_id;
    bool should_encrypt;
    size_t message_length;
    char message[MAX_MESSAGE_LENGTH];
    struct message_slot_channel* next;
} message_slot_channel_t;

// If you use register_chrdev() to register your device, you can assume that minor numbers
// are in the range [0, 255] (i.e., there can be at most 256 different message slots device files)
typedef struct message_slot_device {
    unsigned char minor_number;
    struct message_slot_channel* channels;
    size_t channel_count;
    struct message_slot_device* prev;
    struct message_slot_device* next;
} message_slot_device_t;

static message_slot_device_t* device_list_head = NULL;  // global pointer to all slots


// ===== Device setup =====
struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
    .release = device_release,
};

static int __init message_slot_init(void)
{
    int major = register_chrdev(MSG_SLOT_DEVICE_MAJOR, MSG_SLOT_DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ERR "Registering %s char device failed with %d\n", MSG_SLOT_DEVICE_NAME, major);
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

// ===== Device functions =====

static message_slot_device_t* find_device_by_minor(int minor_number)
{
    message_slot_device_t* device_ptr = device_list_head;
    while (device_ptr != NULL) {
        if (device_ptr->minor_number == minor_number) {
            return device_ptr;
        }
        device_ptr = device_ptr->next;
    }
    return NULL;
}

static int device_release(struct inode* inode, struct file* file)
{
    int minor_number = iminor(inode);
    message_slot_channel_t* channel_ptr;
    message_slot_device_t* current_device = find_device_by_minor(minor_number);
    if (current_device == NULL) {
        return -EINVAL;  // TODO: should suppress?
    }

    // free all channels
    channel_ptr = current_device->channels;
    while (channel_ptr != NULL) {
        message_slot_channel_t* temp = channel_ptr;
        channel_ptr = channel_ptr->next;
        kfree(temp);
    }

    // remove device from global list
    if (current_device->prev != NULL) {
        current_device->prev->next = current_device->next;
    }
    if (current_device->next != NULL) {
        current_device->next->prev = current_device->prev;
    }
    kfree(current_device);
    printk(KERN_INFO "%s: Device with minor number %d closed\n", MSG_SLOT_DEVICE_NAME, minor_number);
    return GENERAL_SUCCESS;
}

static int device_open(struct inode* inode, struct file* file)
{
    /* TODO: You’ll need a data structure to describe individual message slots (device files with different
    minor numbers). In device_open(), the module can check if it has already created a data
    structure for the file being opened, and create one if not. You can get the opened file’s minor
    number using the iminor() kernel function (applied to the struct inode* argument of
    device_open()).*/
    int return_code = GENERAL_FAILURE;
    int minor_number = iminor(inode);
    message_slot_device_t* current_device = find_device_by_minor(minor_number);

    // allocate new device if not found. if found - do nothing.
    if (current_device == NULL) {
        current_device = kmalloc(sizeof(message_slot_device_t), GFP_KERNEL);
        if (current_device == NULL) {
            printk(KERN_ERR "Failed to allocate memory for new message slot device\n");
            return_code = -ENOMEM;
            goto cleanup;
        }
        memset(current_device, 0, sizeof(message_slot_device_t));
        current_device->minor_number = minor_number;

        // add the new device to the global list
        if (device_list_head == NULL) {
            device_list_head = current_device;
        } else {
            message_slot_device_t* temp = device_list_head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = current_device;
            current_device->prev = temp;
        }
    }

    printk(KERN_INFO "%s: Device with minor number %d opened\n", MSG_SLOT_DEVICE_NAME, minor_number);
    return_code = GENERAL_SUCCESS;

cleanup:
    return return_code;
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
    /* TODO: device_ioctl() needs to associate the passed channel id with the file descriptor it was
    invoked on. You can use the void* private_data field in the file structure parameter
    for this purpose. For example: file->private_data = (void*) 3. Check <linux/fs.h>
    for the details on struct file. */
    return 0;
}
