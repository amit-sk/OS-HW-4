#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "message_slot.h"

int main(int argc, char* argv[])
{
    int return_code = GENERAL_FAILURE;
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

    fd = open(filepath, O_RDWR);
    if (-1 == fd) {
        perror("open failed");
        goto cleanup;
    }

    return_code = GENERAL_SUCCESS;
cleanup:
    if (-1 != fd) {
        close(fd);
    }

    return 0;
}
