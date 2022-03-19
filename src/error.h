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

#endif /* ERROR_H */