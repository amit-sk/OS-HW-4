#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message_slot.h"

#define SENDER_SUCCESS (0)
#define SENDER_FAILURE (1)

int main(int argc, char* argv[])
{
    int return_code = SENDER_FAILURE;
    int return_value = GENERAL_FAILURE;  // for device function return values
    char* filepath = NULL;
    unsigned long channel_id = 0;
    unsigned char censorship_mode = 0;
    char* message = NULL;
    char* endptr = NULL;  // for str to int conversion
    int fd = -1;

    if (argc != 5) {
        fprintf(stderr, "Usage: %s <message_slot_file_path> <channel_id> <censorship_mode> <message>\n", argv[0]);
        goto cleanup;
    }

    channel_id = strtoul(argv[2], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Invalid channel id: %s\n", argv[2]);
        goto cleanup;
    }

    endptr = NULL;  // reset
    censorship_mode = (unsigned char)strtoul(argv[3], &endptr, 10);
    if ((*endptr != '\0') || (censorship_mode > 1)) {
        fprintf(stderr, "Invalid censorship mode: %s. expecting 0 or 1.\n", argv[3]);
        goto cleanup;
    }

    filepath = argv[1];
    message = argv[4];

    // Open the specified message slot device file.
    fd = open(filepath, O_RDWR);
    if (-1 == fd) {
        perror("open failed");
        goto cleanup;
    }

    // Set the censorship mode to the value specified on the command line 
    return_value = device_ioctl(fd, MSG_SLOT_SET_CEN, censorship_mode);
    if (GENERAL_SUCCESS != return_value) {
        perror("ioctl failed");
        goto cleanup;
    }

    // Set the channel id to the id specified on the command line.
    return_value = device_ioctl(fd, MSG_SLOT_CHANNEL, channel_id);
    if (GENERAL_SUCCESS != return_value) {
        perror("ioctl failed");
        goto cleanup;
    }

    // Write the specified message to the message slot file.
    return_value = device_write(fd, message, strlen(message), NULL);
    if (GENERAL_SUCCESS != return_value) {
        perror("write failed");
        goto cleanup;
    }

    return_code = SENDER_SUCCESS;
cleanup:
    if (-1 != fd) {
        close(fd);
    }

    return return_code;
}
