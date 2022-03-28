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

#include "sensor_io.h"

#define PERIOD 1

/* Argument indices */
enum argi
{
	/* Program name */
	PROGRAM_NAME,
	/* Path to the I2C bus */
	I2C_PATH,
	/* Path to the host */
	HOST_PATH,
	/* Port number */
	PORT_NUM,
	/* Expected number of arguments */
	EXPECTED_ARGC
};

void usage_msg(char* program);

#endif /* SENSOR_THREAD_H */
