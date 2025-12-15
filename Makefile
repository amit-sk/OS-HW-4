obj-m:=message_slot.o
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

all: module reader sender

reader:
	gcc -O3 -Wall -std=c11 message_reader.c -o message_reader

sender:
	gcc -O3 -Wall -std=c11 message_sender.c -o message_sender

module:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean