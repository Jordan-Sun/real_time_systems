/*
 * sensor_thread.c
 *
 * Reads data from an tsl2561 ambient light sensor on the I2C bus at
 * I2C_PATH, and sends it as a data packet to the socket at SOCKET_PATH.
 *
 * Copyright (C) 2022 Jordan Sun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "sensor_thread.h"
#include "sensor_io.h"
#include "socket.h"

#include <stdio.h>

/*
 * Prints the usage message.
 */
void usage_msg(char *program)
{
	printf("Usage: %s I2C_PATH SOCKET_PATH\n", program);
	printf("Reads data from an tsl2561 ambient light sensor on the ");
	printf("I2C bus at I2C_PATH, and sends it as a data packet to ");
	printf("the socket at SOCKET_PATH.\n");
}

int main(int argc, char *argv[])
{
	/* Return values */
	int ret;
	/* File descriptor of the sensor and the socket */
	int sensor_fd, sock_fd;
	/* Package sequence counter */
	unsigned int seq;
	/* Data packets */
	sensor_packet_t packet, last_packet;
	/* Sleep timestamps */
	struct timespec next_timeout;

	if (argc != EXPECTED_ARGC)
	{
		usage_msg(argv[PROGRAM_NAME]);
		return -ERR_NUM_ARGC;
	}

	/* Initialize the sensor */
	printf("Initializing sensor %s...\n", argv[I2C_PATH]);
	sensor_fd = init_sensor(argv[I2C_PATH]);
	if (sensor_fd < SUCCESS)
		return sensor_fd;
	printf("Initialized.\n");
	
	/* Connect to socket */
	printf("Connecting to socket %s...\n", argv[SOCK_PATH]);
	sock_fd = conn_socket(argv[SOCK_PATH]);
	if (sock_fd < SUCCESS)
		return sock_fd;
	printf("Connected.\n");

	
	ret = clock_gettime(CLOCK_MONOTONIC, &next_timeout);
	if (ret < SUCCESS)
	{
		perror("Failed to get time");
		return ERR_TIME;
	}
	next_timeout.tv_nsec += PERIOD;
	if (next_timeout.tv_nsec >= 1000000000)
	{
		next_timeout.tv_sec += 1;
		next_timeout.tv_nsec -= 1000000000;
	}
	nanosleep(&next_timeout, NULL);
	
	/* First packet */
	ret = read_sensor(sensor_fd, &packet);
	if (ret < SUCCESS)
		return ret;
	seq = 0;
	packet.sequence = 0;
	ret = clock_gettime(CLOCK_MONOTONIC, &packet.timestamp);
	if (ret < SUCCESS)
	{
		perror("Failed to get time");
		return ERR_TIME;
	}
	ret = send_packet(sock_fd, &packet);
	if (ret < SUCCESS)
		return ret;
	last_packet = packet;

	for (;;)
	{	
		next_timeout.tv_nsec += PERIOD;
		if (next_timeout.tv_nsec >= 1000000000)
		{
			next_timeout.tv_sec += 1;
			next_timeout.tv_nsec -= 1000000000;
		}
		nanosleep(&next_timeout, NULL);
		
		ret = read_sensor(sensor_fd, &packet);
		if (ret < SUCCESS)
			return ret;
		if ((packet.infrared != last_packet.infrared) || (packet.full != last_packet.full))
		{
			packet.sequence = ++seq;
			ret = clock_gettime(CLOCK_MONOTONIC, &packet.timestamp);
			if (ret < SUCCESS)
			{
				perror("Failed to get time");
				return ERR_TIME;
			}
			ret = send_packet(sock_fd, &packet);
			if (ret < SUCCESS)
				return ret;
			last_packet = packet;
		}
	}

	return SUCCESS;
}
