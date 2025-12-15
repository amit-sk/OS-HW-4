#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>

#define GENERAL_SUCCESS (0)
#define GENERAL_FAILURE (-1)

#define MSG_SLOT_DEVICE_MAJOR (235)
#define MSG_SLOT_DEVICE_NAME "message_slot"

#define MSG_SLOT_CHANNEL _IOW(MSG_SLOT_DEVICE_MAJOR, 0, unsigned long)
#define MSG_SLOT_SET_CEN _IOW(MSG_SLOT_DEVICE_MAJOR, 1, unsigned long)

#define MAX_MESSAGE_LENGTH (128)

static int device_open(struct inode* inode, struct file* file);
static ssize_t device_read(struct file* file, char __user* buffer, size_t length, loff_t* offset);
static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset);
static long device_ioctl(struct file* file, unsigned int cmd, unsigned long arg);

#endif // MESSAGE_SLOT_H
