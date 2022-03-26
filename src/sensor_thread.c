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
	/* Data packets */
	sensor_packet_t packet, last_packet;

	if (argc != EXPECTED_ARGC)
	{
		usage_msg(argv[PROGRAM_NAME]);
		return -ERR_NUM_ARGC;
	}

	/* Initialize the sensor */
	sensor_fd = init_sensor(argv[I2C_PATH]);
	if (sensor_fd < SUCCESS)
		return sensor_fd;

	/* Discards the first packet */
	ret = read_sensor(sensor_fd, &packet);
	if (ret < SUCCESS)
		return ret;
	last_packet = packet;

	for (;;)
	{
		read_sensor(sensor_fd, &packet);
		if (ret < SUCCESS)
			return ret;
		last_packet = packet;
	}

	return SUCCESS;
}