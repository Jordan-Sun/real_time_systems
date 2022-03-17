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
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <linux/i2c-dev.h>

/* 
 * Prints the usage message.
 */
void usage_msg(char* program)
{
	printf("Usage: %s I2C_PATH SOCKET_PATH\n", program);
	printf("Reads data from an tsl2561 ambient light sensor on the ");
	printf("I2C bus at I2C_PATH, and sends it as a data packet to ");
	printf("the socket at SOCKET_PATH.\n");
}

int init_sensor(const char* bus)
{
	/* stores the fd of the i2c bus */
	int fd = -ERR_OPEN;
	
	/* attempt to open the i2c bus */
	fd = open(bus, O_RDWR);
	if(fd < SUCCESS) 
	{
		perror("Failed to open i2c bus");
		return -ERR_OPEN;
	}

	/* select the sensor */
	if (ioctl(fd, I2C_SLAVE, SENSOR_ADDR) < SUCCESS)
	{
		perror("Failed to find sensor");
		return -ERR_IOCTL;
	}
	
	/* configure the sensor */
	if (write(fd, control_reg, CONTROL_REGC) != CONTROL_REGC)
	{
		perror("Failed to power on sensor");
		return -ERR_WRITE_CONTROL;
	}
	if (write(fd, timing_reg, TIMING_REGC) != TIMING_REGC)
	{
		perror("Failed to set sensor timing");
		return -ERR_WRITE_TIMING;
	}
	
	return fd;
}

int read_sensor(int fd, sensor_packet_t* packet)
{
	char data_reg[DATA_REGC] = {0};
	
	/* Read data from sensor */
	if (write(fd, read_reg, READ_REGC) != READ_REGC)
	{
		perror("Failed to initialize read");
		return -ERR_WRITE_READ;
	}
	
	if(read(fd, data_reg, DATA_REGC) != DATA_REGC)
	{
		perror("Failed to read data");
		return -ERR_READ_DATA;
	}
	
	/* Reformat the data */
	packet->full = (data_reg[FULL_MSB] << 8) + data_reg[FULL_LSB];
	packet->infrared = (data_reg[INFRARED_MSB] << 8) + data_reg[INFRARED_LSB];
	/* Get the visible light intensity by calculating the difference */
	packet->visible = packet->full - packet->infrared;
	
	if (time(&packet->timestamp) < SUCCESS)
	{
		perror("Failed to get timestamp");
		return -ERR_TIME;
	}
	
	return SUCCESS;
}

int init_socket(const char* socket)
{
	/* Socket address name */
	struct sockaddr_un name;
	
	return SUCCESS;
}

int send_packet(sensor_packet_t* packet)
{
	return SUCCESS;
}

int main(int argc, char* argv[])
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
	if (sensor_fd < SUCCESS) return sensor_fd;
	
	/* Discards the first packet */
	ret = read_sensor(sensor_fd, &packet);
	if (ret < SUCCESS) return ret;
	last_packet = packet;
	
	for (;;)
	{
		read_sensor(sensor_fd, &packet);
		if (ret < SUCCESS) return ret;
		last_packet = packet;
	}
	
	return SUCCESS;
}
