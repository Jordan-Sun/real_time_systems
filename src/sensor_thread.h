/*
 * sensor_thread.h
 *
 * Reads data from an tsl2561 ambient light sensor on the I2C bus at 
 * I2C_PATH, and sends it as a data packet to the socket at SOCKET_PATH.
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

#ifndef SENSOR_THREAD_H
#define SENSOR_THREAD_H

#include "sensor_packet.h"

/* The sensor uses a fixed I2C address of 0x39. */
#define SENSOR_ADDR 0x39

/* Registers */
#define CONTROL_REGC 2
/* Select control register (0x80) */
/* Set power on mode (0x03) */
const char control_reg[CONTROL_REGC] = {0x80, 0x03};

#define TIMING_REGC 2
/* Select timing register (0x81) */
/* Set 402ms resolution  (0x02) */
const char timing_reg[TIMING_REGC] = {0x81, 0x02};

#define READ_REGC 1
/* Select read register (0x8c) */
const char read_reg[READ_REGC] = {0x8c};

#define DATA_REGC 4
/* Data indices */
enum datai
{
	/* Least significant byte of the full intensity */
	FULL_LSB,
	/* Most significant byte of the full intensity */
	FULL_MSB,
	/* Least significant byte of the infrared intensity */
	INFRARED_LSB,
	/* Most significant byte of the infrared intensity */
	INFRARED_MSB,
};

/* Socket protocal */
#define SOCK_PROTOCAL 0

/* Argument indices */
enum argi
{
	/* Program name */
	PROGRAM_NAME,
	/* Path to the I2C bus */
	I2C_PATH,
	/* Path to the socket */
	SOCKET_PATH,
	/* Expected number of arguments */
	EXPECTED_ARGC
};

/* Error codes (negated) */
enum errcode
{
	/* Success (no error) */
	SUCCESS,
	/* Unexpected number of arguments */
	ERR_NUM_ARGC,
	/* Failed to open i2c bus fd */
	ERR_OPEN,
	/* Failed to configure i2c bus fd */
	ERR_IOCTL,
	/* Failed to read or write to the i2c bus fd */
	ERR_WRITE_CONTROL,
	ERR_WRITE_TIMING,
	ERR_WRITE_READ,
	ERR_READ_DATA,
	/* Failed to create the socket */
	/* Failed to connect to the socket */
	/* Failed to read or write to the i2c bus fd */
	/* Get time failure */
	ERR_TIME,
};

void usage_msg(char* program);

int init_sensor(const char* bus);

int read_sensor(int fd, sensor_packet_t* packet);

int init_socket(const char* socket);

int send_packet(sensor_packet_t* packet);

#endif /* SENSOR_THREAD_H */
