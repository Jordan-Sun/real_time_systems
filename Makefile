CC=gcc
CFLAGS=-Wall -Wextra -O3

all: out/sensor_thread_unix out/sensor_thread_inet out/control_thread_unix out/control_thread_inet

out/sensor_thread_unix: src/sensor_thread_unix.c src/socket_unix.c src/sensor_io.c src/blocking_io.c
	$(CC) $^ -o $@ $(CFLAGS)

out/sensor_thread_inet: src/sensor_thread_inet.c src/socket_unix.c src/sensor_io.c src/blocking_io.c
	$(CC) $^ -o $@ $(CFLAGS)

out/control_thread_unix: src/control_thread_unix.c src/socket_unix.c
	$(CC) $^ -o $@ $(CFLAGS)

out/control_thread_inet: src/control_thread_inet.c src/socket_inet.c
	$(CC) $^ -o $@ $(CFLAGS)

test: out/test_blocking_io out/test_socket_client out/test_socket_server

out/test_blocking_io: test/test_blocking_io.c src/blocking_io.c
	$(CC) $^ -o $@ $(CFLAGS)

out/test_socket_client: test/test_socket_client.c src/blocking_io.c src/socket_unix.c
	$(CC) $^ -o $@ $(CFLAGS)

out/test_socket_server: test/test_socket_server.c src/blocking_io.c src/socket_unix.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm out/*
