CC=gcc
CFLAGS=-Wall -Wextra

all: sensor_thread
sensor_thread: src/sensor_thread.c
	$(CC) $< -o $@ $(CFLAGS)
