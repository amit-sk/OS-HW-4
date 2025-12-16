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
    unsigned int channel_id;
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

static int device_open(struct inode* inode, struct file* file);
static ssize_t device_read(struct file* file, char __user* buffer, size_t length, loff_t* offset);
static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset);
static long device_ioctl(struct file* file, unsigned int cmd, unsigned long arg);
static int device_release(struct inode* inode, struct file* file);

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
    // requesting specific major
    int major = register_chrdev(MSG_SLOT_DEVICE_MAJOR, MSG_SLOT_DEVICE_NAME, &fops);
    if (major != 0) {
        printk(KERN_ERR "Registering %s char device as major %d failed with %d\n", MSG_SLOT_DEVICE_NAME, MSG_SLOT_DEVICE_MAJOR, major);
        return major;
    }
    printk(KERN_INFO "%s: Device with major number %d - registered\n", MSG_SLOT_DEVICE_NAME, MSG_SLOT_DEVICE_MAJOR);
    return DRIVER_SUCCESS;
}

static void free_all_channels(message_slot_channel_t* channel_ptr)
{
    while (channel_ptr != NULL) {
        message_slot_channel_t* temp_channel = channel_ptr;
        channel_ptr = channel_ptr->next;
        kfree(temp_channel);
    }
}
static void free_all(void)
{
    message_slot_device_t* device_ptr = device_list_head;
    while (device_ptr != NULL) {
        message_slot_device_t* next_device = device_ptr->next;
        free_all_channels(device_ptr->channels);
        kfree(device_ptr);
        device_ptr = next_device;
    }
}

static void __exit message_slot_exit(void)
{
    unregister_chrdev(MSG_SLOT_DEVICE_MAJOR, MSG_SLOT_DEVICE_NAME);  // best effort, not checking return value
    printk(KERN_INFO "%s: Device with major number %d - unregistered\n", MSG_SLOT_DEVICE_NAME, MSG_SLOT_DEVICE_MAJOR);

    // all memory is kept until the module is unloaded.
    printk(KERN_INFO "%s: Freeing all memory\n", MSG_SLOT_DEVICE_NAME);
    free_all();
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

/*
 * all memory is kept until the module is unloaded.
 * closing a file (release) will not free the device structure, so subsequent opens will find it.
*/
static int device_release(struct inode* inode, struct file* file)
{
    int minor_number = iminor(inode);
    printk(KERN_INFO "%s: Device with minor number %d closed\n", MSG_SLOT_DEVICE_NAME, minor_number);
    return DRIVER_SUCCESS;
}

/*
 * initializes a new channel for the given file associated with the given device.
 * sets the file's private_data to point to the new channel, and adds the channel to the device's channel list.
*/
static int init_channel(struct file* file, message_slot_device_t* device)
{
    int return_code = DRIVER_FAILURE;
    message_slot_channel_t* new_channel = NULL;
    message_slot_channel_t* channel_ptr = NULL;

    if (NULL == device || NULL == file) {
        goto cleanup;
    }

    // TODO: debug
    printk(KERN_INFO "Initializing new channel for device with minor number %d\n", device->minor_number);

    new_channel = kmalloc(sizeof(message_slot_channel_t), GFP_KERNEL);
    if (NULL == new_channel) {
        printk(KERN_ERR "Failed to allocate memory for initial channel\n");
        return_code = -ENOMEM;
        goto cleanup;
    }
    memset(new_channel, 0, sizeof(message_slot_channel_t));

    file->private_data = (void*)new_channel;  // set in file

    // set in device channel list
    if (NULL == device->channels) {
        // was empty
        device->channels = new_channel;
        device->channel_count = 1;
    } else {
        channel_ptr = device->channels;
        while (channel_ptr->next != NULL) {
            channel_ptr = channel_ptr->next;
        }
        channel_ptr->next = new_channel;
        device->channel_count++;
    }

    return_code = DRIVER_SUCCESS;
cleanup:
    return return_code;
}

static int device_open(struct inode* inode, struct file* file)
{
    int return_code = DRIVER_FAILURE;
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

    // allocate channels for file and set in private_data
    return_code = init_channel(file, current_device);
    if (DRIVER_SUCCESS != return_code) {
        goto cleanup;
    }

    printk(KERN_INFO "%s: Device with minor number %d opened\n", MSG_SLOT_DEVICE_NAME, minor_number);
    return_code = DRIVER_SUCCESS;
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
    int return_code = DRIVER_FAILURE;
    message_slot_channel_t* current_channel = (message_slot_channel_t*)file->private_data;  // the channel associated with the file

    if (NULL == current_channel) {
        // this shouldn't happen since the channel associated with a file is initialized on open
        printk(KERN_ERR "ioctl called on non-initialized file\n");
        return_code = -EINVAL;
        goto cleanup;
    }

    // TODO: debug
    printk(KERN_INFO "%s: ioctl called with cmd %u and arg %lu\n", MSG_SLOT_DEVICE_NAME, cmd, arg);

    switch (cmd)
    {
    case MSG_SLOT_CHANNEL:
        // channel id expected to be a non-zero unsigned int
        if (arg == 0) {
            printk(KERN_ERR "ioctl called with invalid channel id 0\n");
            return_code = -EINVAL;
            goto cleanup;
        }
        current_channel->channel_id = (unsigned int)arg;
        break;

    case MSG_SLOT_SET_CEN:
        // censorship mode expected to be 0 or 1
        if (arg > 1) {
            printk(KERN_ERR "ioctl called with invalid censorship mode %lu\n", arg);
            return_code = -EINVAL;
            goto cleanup;
        }
        current_channel->should_encrypt = (bool)arg;
        break;
    
    default:
        printk(KERN_ERR "ioctl called with invalid command %lu\n", arg);
        return_code = -EINVAL;
        goto cleanup;
        break;
    }

    return_code = DRIVER_SUCCESS;
cleanup:
    return return_code;
}
