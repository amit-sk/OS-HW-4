#!/bin/bash

echo "Running message slot tests..."
echo "inserts and removes the module on its own to ensure a clean state"
echo
sudo rmmod message_slot.ko 2> /dev/null
sudo insmod message_slot.ko
sudo mknod /dev/test0 c 235 0
sudo mknod /dev/test1 c 235 1
sudo chmod 0777 /dev/test0
sudo chmod 0777 /dev/test1
gcc -O3 -Wall -std=c11 ex4_tester.c -o tester
./tester
sudo rm /dev/test0
sudo rm /dev/test1
sudo rmmod message_slot.ko
