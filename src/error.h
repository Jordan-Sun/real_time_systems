/*
 * error.h
 *
 * Error code produced by the program.
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

#ifndef ERROR_H
#define ERROR_H

/* Error codes (negated) */
enum errcode
{
    /* Success (no error) */
    SUCCESS,
    /* Unexpected number of arguments */
    ERR_NUM_ARGC,
    /* Failed to open i2c bus fd */
    ERR_OPEN = 10,
    /* Failed to configure i2c bus fd */
    ERR_IOCTL,
    /* Failed to write control configuration to the i2c bus fd */
    ERR_WRITE_CONTROL,
    /* Failed to write timing configuration to the i2c bus fd */
    ERR_WRITE_TIMING,
    /* Failed to write read instruction to the i2c bus fd */
    ERR_WRITE_READ,
    /* Failed to read data from the i2c bus fd */
    ERR_READ_DATA,
    /* Get time failure */
    ERR_TIME,
    /* Failed to create socket */
    ERR_SOCKET = 20,
    /* Failed to bind the socket */
    ERR_BIND,
    /* Failed to listen to socket */
    ERR_LISTEN,
    /* Failed to connect to the socket */
    ERR_CONNECT,
    /* Failed to send to the socket */
    ERR_SEND,
};

#endif /* ERROR_H */