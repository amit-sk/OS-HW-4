#include "message_slot.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <sys/ioctl.h>

static char* DEV0 = "/dev/test0";
static char* DEV1 = "/dev/test1";

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
void test8();
void test9();
void test10();
void test11();
void test12();
void test13();
void test14();
void test15();
void test16();
void test17();
void test18();
void test19();
void test20();
void test21();
void test22();
void test23();
void test24();
void test25();
void test26();
void print_failure(int test_num);
void print_success(int test_num);
void encrypt_message(char* message, size_t length);

int main(void)
{
	printf("RESULTS\n-------\n");
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
	test8();
	test9();
	test10();
	test11();
	test12();
	test13();
	test14();
	test15();
	test16();
	test17();
	test18();
	test19();
	test20();
	test21();
	test22();
	test23();
	test24();
	test25();
	test26();

	printf("DONE!\n");

	return 0;
}

/* 
 * open device0, set channel to 6, write "Hello World!", read it back and verify
 */
void test1()
{
	int device0_fd;
	size_t bytes_written;
	size_t bytes_read;
	char msg[128];

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{
		perror("open failed");
		printf("Failed to open device %s\n", DEV0);
		print_failure(1); exit(0);
	}

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 6) < 0)
	{
		perror("ioctl failed");
		printf("Failed to set channel %d on device %s\n", 6, DEV0);
		print_failure(1); exit(0); }

	bytes_written = write(device0_fd, "Hello World!", 12);
	if (bytes_written < 12)
	{
		perror("write failed");
		printf("Failed to write message to device %s\n", DEV0);
		print_failure(1); exit(0); }

	bytes_read = read(device0_fd, msg, sizeof(msg));
	if (bytes_read < 12)
	{
		perror("read failed");
		printf("Failed to read message from device %s\n", DEV0);
		print_failure(1); exit(0); }

	msg[12] = '\0';
	if (strcmp(msg, "Hello World!"))
	{
		printf("Read incorrect message from device %s: %s\n", DEV0, msg);
		print_failure(1); exit(0); }

	print_success(1);
}

/*
 * open device0 and device1, set channel to 99, write "dev0" and "dev1" respectively, read them back and verify 
 */
void test2()
{
        int device0_fd;
	int device1_fd;
        size_t bytes_written;
        size_t bytes_read;
        char msg[128];

        device0_fd = open(DEV0, O_RDWR);
        if (device0_fd < 0)
        { print_failure(2); exit(0); }

        device1_fd = open(DEV1, O_RDWR);
        if (device1_fd < 0)
        { print_failure(2); exit(0); }

        if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 99) < 0)
        { print_failure(2); exit(0); }

        if (ioctl(device1_fd, MSG_SLOT_CHANNEL, 99) < 0)
        { print_failure(2); exit(0); }

        bytes_written = write(device0_fd, "dev0", 4);
        if (bytes_written < 4)
        { print_failure(2); exit(0); }

        bytes_written = write(device1_fd, "dev1", 4);
        if (bytes_written < 4)
        { print_failure(2); exit(0); }

        bytes_read = read(device0_fd, msg, sizeof(msg));
        if (bytes_read < 4)
        { print_failure(2); exit(0); }

        msg[4] = '\0';
        if (strcmp(msg, "dev0"))
        { print_failure(2); exit(0); }

        bytes_read = read(device1_fd, msg, 128);
        if (bytes_read < 4)
        { print_failure(2); exit(0); }

        msg[4] = '\0';
        if (strcmp(msg, "dev1"))
        { print_failure(2); exit(0); }

	close(device0_fd);
	close(device1_fd);

        print_success(2);

}

/* 
 * open device0, set channel to 6, read back "Hello World!" written in test1 and verify
 */
void test3()
{
        int device0_fd;
        size_t bytes_read;
        char msg[128];

        device0_fd = open(DEV0, O_RDWR);
        if (device0_fd < 0)
        { print_failure(3); exit(0); }

        if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 6) < 0)
        { print_failure(3); exit(0); }

        bytes_read = read(device0_fd, msg, 128);
        if (bytes_read < 12)
        { print_failure(3); exit(0); }

	msg[12] = '\0';
	if (strcmp(msg, "Hello World!"))
	{ print_failure(3); exit(0); }

	close(device0_fd);

	print_success(3);
}

/* 
 * open device0 and device1, set channel to 6 on device0, try to write to device1 - should fail
 */
void test4()
{
	int device0_fd;
	int device1_fd;

        device0_fd = open(DEV0, O_RDWR);
        if (device0_fd < 0)
        { print_failure(4); exit(0); }

        device1_fd = open(DEV1, O_RDWR);
        if (device1_fd < 0)
        { print_failure(4); exit(0); }

        if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 6) < 0)
        { print_failure(4); exit(0); }

	if (write(device1_fd, "hey", 3) >= 0)
	{ print_failure(4); exit(0); }

        close(device0_fd);
	close(device1_fd);

        print_success(4);
}

/* 
 * open device0 and device1, set channel to 6 on device0, try to write to device1 - should fail
 * (same as test4 but different order of commands)
 */
void test5()
{
        int device0_fd;
        int device1_fd;

        device0_fd = open(DEV0, O_RDWR);
        if (device0_fd < 0)
        { print_failure(5); exit(0); }

        if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 6) < 0)
        { print_failure(5); exit(0); }

	device1_fd = open(DEV1, O_RDWR);
	if (device1_fd < 0)
	{ print_failure(5); exit(0); }

        if (write(device1_fd, "hey", 3) >= 0)
        { print_failure(5); exit(0); }

        close(device0_fd);
        close(device1_fd);

	print_success(5);
}

/* 
 * open device0, set channel to 1024, write "old", write "new", read back and verify "new" is read
 */
void test6()
{
	int device0_fd;
	char msg[128];
	int bytes_written;
	int bytes_read;

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(6); exit(0); }

        if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 1024) < 0)
        { print_failure(6); exit(0); }

	bytes_written = write(device0_fd, "old", 3);
	if (bytes_written < 3)
	{ print_failure(6); exit(0); }

	bytes_written = write(device0_fd, "new", 3);
	if (bytes_written < 3)
	{ print_failure(6); exit(0); }

	bytes_read = read(device0_fd, msg, 128);
	if (bytes_read < 3)
	{ print_failure(6); exit(0); }

	msg[3] = '\0';
	if (strcmp(msg, "new"))
	{ print_failure(6); exit(0); }

	bytes_read = read(device0_fd, msg, 128);
	if (bytes_read < 3)
	{ print_failure(6); exit(0); }

	msg[3] = '\0';
	if (strcmp(msg, "new"))
	{ print_failure(6); exit(0); }

	close(device0_fd);

	print_success(6);
}

struct junk{
int var1;
char var2;
char *var3;
};

/* 
 * open device0, set channel to 161616, write junk struct, read it back and verify
 */
void test7()
{
	struct junk *junk;
	int device0_fd;
	int bytes_written;
	int bytes_read;

	junk = (struct junk *) malloc(sizeof(struct junk));
	if (junk == NULL)
	{ print_failure(7); exit(0); }

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(7); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 161616) < 0)
	{ print_failure(7); exit(0); }

	bytes_written = write(device0_fd, junk, sizeof(struct junk));
	if (bytes_written < sizeof(struct junk))
	{ print_failure(7); exit(0); }

	bytes_read = read(device0_fd, junk, sizeof(struct junk));
	if (bytes_read < sizeof(struct junk))
	{ print_failure(7); exit(0); }

	close(device0_fd);

	print_success(7);
}

/*
 * open device0, set channel to 199, read back and verify no message is available
 */
void test8()
{
        int device0_fd;
	char msg[128];

        device0_fd = open(DEV0, O_RDWR);
        if (device0_fd < 0)
        { print_failure(8); exit(0); }

        if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 199) < 0)
        { print_failure(8); exit(0); }

	if (read(device0_fd, msg, 128) != -1 || errno != EWOULDBLOCK)
	{ print_failure(8); exit(0); }

        close(device0_fd);

        print_success(8);
}

/* 
 * open device0, set channel to 54, try to write zero-length message - should fail
 */
void test9()
{
	int device0_fd;
	size_t bytes_written;

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(9); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 54) < 0)
	{ print_failure(9); exit(0); }

	bytes_written = write(device0_fd, "", 0);
	if (bytes_written != -1 || errno != EMSGSIZE)
	{ print_failure(9); exit(0); }

	close(device0_fd);

	print_success(9);
}

/* 
 * open device0, set channel to 2049, try to write 129-byte message - should fail, message too long
 */
void test10()
{
        int device0_fd;
        size_t bytes_written;
	char *msg = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

        device0_fd = open(DEV0, O_RDWR);
        if (device0_fd < 0)
        { print_failure(10); exit(0); }

        if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 2049) < 0)
        { print_failure(10); exit(0); }

        bytes_written = write(device0_fd, msg, 129);
        if (bytes_written != -1 || errno != EMSGSIZE)
        { print_failure(10); exit(0); }

        close(device0_fd);

        print_success(10);
}

/* 
 * open device0, try to write before setting channel - should fail
 */
void test11()
{
	int device0_fd;
	size_t bytes_written;

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(11); exit(0); }

	bytes_written = write(device0_fd, "abcd", 4);
	if (bytes_written != -1 || errno != EINVAL)
	{ print_failure(11); exit(0); }

	close(device0_fd);

	print_success(11);
}

/* 
 * open device0, try to read before setting channel - should fail
 */
void test12()
{
        int device0_fd;
        size_t bytes_read;

        device0_fd = open(DEV0, O_RDWR);
        if (device0_fd < 0)
        { print_failure(12); exit(0); }

        bytes_read = read(device0_fd, "abcd", 4);
        if (bytes_read != -1 || errno != EINVAL)
        { print_failure(12); exit(0); }

        close(device0_fd);

        print_success(12);
}

/* 
 * open device0, set channel to 999, write "testtest", try to read into 4-byte buffer - should fail, buffer too small
 */
void test13()
{
	int device0_fd;
	char msg[4];
	int bytes_written;
	int bytes_read;

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(13); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 999) < 0)
	{ print_failure(13); exit(0); }

	bytes_written = write(device0_fd, "testtest", 8);
	if (bytes_written < 8)
	{ print_failure(13); exit(0); }

	bytes_read = read(device0_fd, msg, 4);
	if (bytes_read != -1 || errno != ENOSPC)
	{ print_failure(13); exit(0); }

	close(device0_fd);

	print_success(13);
}

/* 
 * open device0, set channel to 1048578 (large channel id), should succeed
 */
void test14()
{
	int device0_fd;

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(14); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 1048578) < 0)
	{ print_failure(14); exit(0); }

	close(device0_fd);

	print_success(14);
}

/* 
 * open device0, set channel to 6, write ecypted message, read back and verify censorship
 */
void test15()
{
	int device0_fd;
	char orig_msg[] = "This is a secret message.";
	char msg[128];

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(15); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 6) < 0)
	{ print_failure(15); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 1) < 0)
	{ print_failure(15); exit(0); }

	if (write(device0_fd, "This is a secret message.", 25) < 25)
	{ print_failure(15); exit(0); }

	encrypt_message(orig_msg, 25);

	if (read(device0_fd, msg, 128) < 0)
	{ print_failure(15); exit(0); }

	msg[25] = '\0';
	if (strcmp(msg, orig_msg))
	{ print_failure(15); exit(0); }

	close(device0_fd);

	print_success(15);
}

/* 
 * turn encryption on and off on a channel, verify messages are censored or not accordingly
 */
void test16()
{
	int device0_fd;
	char orig_msg_for_enc_on[] = "Encryption ON test.";
	char orig_msg_for_enc_off[] = "Encryption OFF test.";
	char msg[128] = {0};

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(16); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 7) < 0)
	{ print_failure(16); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 1) < 0)
	{ print_failure(16); exit(0); }

	if (write(device0_fd, orig_msg_for_enc_on, strlen(orig_msg_for_enc_on)) < strlen(orig_msg_for_enc_on))
	{ print_failure(16); exit(0); }

	encrypt_message(orig_msg_for_enc_on, strlen(orig_msg_for_enc_on));

	if (read(device0_fd, msg, 128) < 0)
	{ print_failure(16); exit(0); }

	msg[strlen(orig_msg_for_enc_on)] = '\0';
	if (strcmp(msg, orig_msg_for_enc_on))
	{ print_failure(16); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 0) < 0)
	{ print_failure(16); exit(0); }

	if (write(device0_fd, orig_msg_for_enc_off, strlen(orig_msg_for_enc_off)) < strlen(orig_msg_for_enc_off))
	{ print_failure(16); exit(0); }

	if (read(device0_fd, msg, 128) < 0)
	{ print_failure(16); exit(0); }

	msg[strlen(orig_msg_for_enc_off)] = '\0';
	if (strcmp(msg, orig_msg_for_enc_off))
	{ print_failure(16); exit(0); }

	close(device0_fd);

	print_success(16);
}

/*
 * turn encryption on then off without writing a message, verify enc off.
 */
void test17()
{
	int device0_fd;
	char orig_msg[] = "No Encryption";

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(17); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 8) < 0)
	{ print_failure(17); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 1) < 0)
	{ print_failure(17); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 0) < 0)
	{ print_failure(17); exit(0); }

	if (write(device0_fd, orig_msg, strlen(orig_msg)) < strlen(orig_msg))
	{ print_failure(17); exit(0); }

	char msg[128] = {0};
	if (read(device0_fd, msg, sizeof(msg)) < 0)
	{ print_failure(17); exit(0); }

	msg[strlen(orig_msg)] = '\0';
	if (strcmp(msg, orig_msg))
	{ print_failure(17); exit(0); }

	close(device0_fd);

	print_success(17);
}

/* 
 * open device0, set channel to 88, turn encryption on, try to read without writing - should fail, no message
 */
void test18()
{
	int device0_fd;
	int bytes_read;
	char msg[128] = {0};

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(18); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 88) < 0)  // need unused channel
	{ print_failure(18); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 1) < 0)
	{ print_failure(18); exit(0); }

	bytes_read = read(device0_fd, msg, sizeof(msg));
	if (bytes_read != -1 || errno != EWOULDBLOCK)
	{
		perror("read failed");
		printf("read should have failed due to no message available, errno EWOULDBLOCK (%d)\n", EWOULDBLOCK);
		printf("instead returned %d bytes, errno %d\n", bytes_read, errno);
		print_failure(18); exit(0); }

	close(device0_fd);

	print_success(18);
}	

/*
 * turn enc on before setting channel - should succeed
*/
void test19()
{
	int device0_fd;

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(19); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 1) < 0)
	{ print_failure(19); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 0) < 0)
	{ print_failure(19); exit(0); }

	close(device0_fd);

	print_success(19);
}

/*
 * turn enc on before setting channel, write and read message
*/
void test20()
{
	int device0_fd;
	char orig_msg[] = "Encrypt me!";

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(20); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 1) < 0)
	{ print_failure(20); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 10) < 0)
	{ print_failure(20); exit(0); }

	if (write(device0_fd, orig_msg, strlen(orig_msg)) < strlen(orig_msg))
	{ print_failure(20); exit(0); }
	
	char msg[128] = {0};
	if (read(device0_fd, msg, 128) < 0)
	{ print_failure(20); exit(0); }

	encrypt_message(orig_msg, strlen(orig_msg));

	msg[strlen(orig_msg)] = '\0';
	if (strcmp(msg, orig_msg))
	{ print_failure(20); exit(0); }

	close(device0_fd);

	print_success(20);
}

/*
 * turn enc off before setting channel, write and read message
*/
void test21()
{
	int device0_fd;
	char orig_msg_enc_off[] = "Do not encrypt this!";

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(21); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 0) < 0)
	{ print_failure(21); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 11) < 0)
	{ print_failure(21); exit(0); }

	if (write(device0_fd, orig_msg_enc_off, strlen(orig_msg_enc_off)) < strlen(orig_msg_enc_off))
	{ print_failure(21); exit(0); }

	char msg[128] = {0};
	if (read(device0_fd, msg, sizeof(msg)) < 0)
	{ print_failure(21); exit(0); }

	msg[strlen(orig_msg_enc_off)] = '\0';
	if (strcmp(msg, orig_msg_enc_off))
	{ print_failure(21); exit(0); }

	close(device0_fd);

	print_success(21);
}

/* 
 * set to invalid channel id 0 and verify failure
 */
void test22()
{
	int device0_fd;

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(22); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 0) != -1 || errno != EINVAL)
	{ print_failure(22); exit(0); }

	close(device0_fd);

	print_success(22);
}

/*
 * successfully write and read exactly MAX_MESSAGE_LENGTH bytes
*/
void test23()
{
	int device0_fd;
	char orig_msg[MAX_MESSAGE_LENGTH];
	char msg[MAX_MESSAGE_LENGTH] = {0};
	size_t i;

	// prepare a message of exactly MAX_MESSAGE_LENGTH bytes
	for (i = 0; i < MAX_MESSAGE_LENGTH; i++) {
		orig_msg[i] = 'A' + (i % 26);  // fill with letters A-Z
	}

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(23); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 12) < 0)
	{ print_failure(23); exit(0); }

	if (write(device0_fd, orig_msg, MAX_MESSAGE_LENGTH) < MAX_MESSAGE_LENGTH)
	{ print_failure(23); exit(0); }

	if (read(device0_fd, msg, sizeof(msg)) < 0)
	{ print_failure(23); exit(0); }

	if (memcmp(msg, orig_msg, MAX_MESSAGE_LENGTH) != 0)
	{ print_failure(23); exit(0); }

	close(device0_fd);

	print_success(23);
}

/*
 * test with binary data including null bytes
*/
void test24()
{
	int device0_fd;
	char orig_msg[] = {0x00, 0xFF, 0x7A, 0x3C, 0x00, 0x12, 0x34, 0x56};
	char msg[sizeof(orig_msg)] = {0};

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(24); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 13) < 0)
	{ print_failure(24); exit(0); }

	if (write(device0_fd, orig_msg, sizeof(orig_msg)) < sizeof(orig_msg))
	{ print_failure(24); exit(0); }

	if (read(device0_fd, msg, sizeof(msg)) < 0)
	{ print_failure(24); exit(0); }

	if (memcmp(msg, orig_msg, sizeof(orig_msg)) != 0)
	{ print_failure(24); exit(0); }

	close(device0_fd);

	print_success(24);
}

/*
 * encrypt binary data
*/
void test25()
{
	int device0_fd;
	char orig_msg[] = {0x00, 0xFF, 0x7A, 0x3C, 0x00, 0x12, 0x34, 0x56};
	char expected_encrypted_msg[] = {0x00, 0xFF, 0x7A, '#', 0x00, 0x12, 0x34, '#'};
	char msg[sizeof(orig_msg)] = {0};

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(25); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 14) < 0)
	{ print_failure(25); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 1) < 0)
	{ print_failure(25); exit(0); }

	if (write(device0_fd, orig_msg, sizeof(orig_msg)) < sizeof(orig_msg))
	{ print_failure(25); exit(0); }

	if (read(device0_fd, msg, sizeof(msg)) < 0)
	{ print_failure(25); exit(0); }

	if (memcmp(msg, expected_encrypted_msg, sizeof(orig_msg)) != 0)
	{ print_failure(25); exit(0); }

	close(device0_fd);

	print_success(25);
}

/* 
 * test message too short to be encrypted
*/
void test26()
{
	int device0_fd;
	char orig_msg[] = "abc";  // length 3, less than 4
	char msg[MAX_MESSAGE_LENGTH] = {0};

	device0_fd = open(DEV0, O_RDWR);
	if (device0_fd < 0)
	{ print_failure(26); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_CHANNEL, 15) < 0)
	{ print_failure(26); exit(0); }

	if (ioctl(device0_fd, MSG_SLOT_SET_CEN, 1) < 0)
	{ print_failure(26); exit(0); }

	if (write(device0_fd, orig_msg, strlen(orig_msg)) < strlen(orig_msg))
	{ print_failure(26); exit(0); }

	if (read(device0_fd, msg, sizeof(msg)) < 0)
	{ print_failure(26); exit(0); }

	if (memcmp(msg, orig_msg, sizeof(orig_msg)) != 0)
	{ print_failure(26); exit(0); }

	close(device0_fd);

	print_success(26);
}

void print_success(int test_num)
{
	printf("TEST %d: Success\n", test_num);
}

void print_failure(int test_num)
{
	printf("TEST %d: Failure\n", test_num);
}

void encrypt_message(char* message, size_t length)
{
    size_t i = 0;

    // The censorship algorithm replaces every 4th character (e.g., positions 3, 7, 11, etc., using 0-based indexing) with ’#’. 
    for (i = 3; i < length; i+=4) {
        message[i] = '#';
    }
}
