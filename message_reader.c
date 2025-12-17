#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message_slot.h"

#define READER_SUCCESS (0)
#define READER_FAILURE (1)

int main(int argc, char* argv[])
{
    int return_code = READER_FAILURE;
    int return_value = DRIVER_FAILURE;  // for device function return values
    char* filepath = NULL;
    unsigned int channel_id = 0;
    char message[MAX_MESSAGE_LENGTH] = {0};
    int message_length = 0;
    char* endptr = NULL;  // for str to int conversion
    int fd = -1;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <message_slot_file_path> <channel_id>\n", argv[0]);
        goto cleanup;
    }

    channel_id = (unsigned int)strtoul(argv[2], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Invalid channel id: %s\n", argv[2]);
        goto cleanup;
    }

    filepath = argv[1];

    // Open the specified message slot device file.
    fd = open(filepath, O_RDWR);
    if (-1 == fd) {
        perror("open failed");
        goto cleanup;
    }

    // Set the channel id to the id specified on the command line.
    return_value = ioctl(fd, MSG_SLOT_CHANNEL, channel_id);
    if (DRIVER_FAILURE == return_value) {
        perror("ioctl failed");
        goto cleanup;
    }

    // Read a message from the message slot file to a buffer.
    message_length = read(fd, message, sizeof(message));
    if (DRIVER_FAILURE == message_length) {
        perror("read failed");
        goto cleanup;
    }

    // Print the message to standard output.
    if (message_length != write(STDOUT_FILENO, message, message_length)) {
        perror("write failed");
        goto cleanup;
    }

    return_code = READER_SUCCESS;
cleanup:
    if (-1 != fd) {
        close(fd);
    }

    return return_code;
}
