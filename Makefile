CC=gcc
CFLAGS=-Wall -Wextra -O2

all: out/sensor_unix_thread
out/sensor_unix_thread: src/sensor_thread.c src/sensor_io.c src/socket_unix.c src/blocking_io.c
	$(CC) $^ -o $@ $(CFLAGS)

test: out/test_blocking_io
out/test_blocking_io: test/test_blocking_io.c src/blocking_io.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm out/*