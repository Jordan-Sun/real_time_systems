/*
 * sensor_io.h
 *
 * Configures and reads data from an tsl2561 ambient light sensor.
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

#ifndef SENSOR_IO_H
#define SENSOR_IO_H

#include "sensor_packet.h"
#include "error.h"

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

int init_sensor(const char *bus);

int read_sensor(int fd, sensor_packet_t *packet);

#endif /* SENSOR_IO_H */