#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>

#define DRIVER_SUCCESS (0)
#define DRIVER_FAILURE (-1)

#define MSG_SLOT_DEVICE_MAJOR (235)
#define MSG_SLOT_DEVICE_NAME ("message_slot")
#define MSG_SLOT_DEVICE_FILE_NAME ("msg_slot")
#define MSG_SLOT_DEVICE_FILE_PATH ("/dev/" MSG_SLOT_DEVICE_FILE_NAME)

#define MSG_SLOT_CHANNEL _IOW(MSG_SLOT_DEVICE_MAJOR, 0, unsigned long)
#define MSG_SLOT_SET_CEN _IOW(MSG_SLOT_DEVICE_MAJOR, 1, unsigned long)

#define MAX_MESSAGE_LENGTH (128)

#endif // MESSAGE_SLOT_H
