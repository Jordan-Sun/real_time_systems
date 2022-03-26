/*
 * sensor_io.c
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

#include "sensor_io.h"
#include "blocking_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int init_sensor(const char *bus)
{
    /* stores the fd of the i2c bus */
    int fd = -ERR_OPEN;

    /* attempt to open the i2c bus */
    fd = open(bus, O_RDWR);
    if (fd < SUCCESS)
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
    if (blocking_write(fd, control_reg, CONTROL_REGC) != CONTROL_REGC)
    {
        perror("Failed to power on sensor");
        return -ERR_WRITE_CONTROL;
    }
    if (blocking_write(fd, timing_reg, TIMING_REGC) != TIMING_REGC)
    {
        perror("Failed to set sensor timing");
        return -ERR_WRITE_TIMING;
    }

    return fd;
}

int read_sensor(int fd, sensor_packet_t *packet)
{
    char data_reg[DATA_REGC] = {0};

    /* Read data from sensor */
    if (write(fd, read_reg, READ_REGC) != READ_REGC)
    {
        perror("Failed to initialize read");
        return -ERR_WRITE_READ;
    }

    if (read(fd, data_reg, DATA_REGC) != DATA_REGC)
    {
        perror("Failed to read data");
        return -ERR_READ_DATA;
    }

    /* Reformat the data */
    packet->full = (data_reg[FULL_MSB] << 8) + data_reg[FULL_LSB];
    packet->infrared = (data_reg[INFRARED_MSB] << 8) + data_reg[INFRARED_LSB];
    /* Get the visible light intensity by calculating the difference */
    packet->visible = packet->full - packet->infrared;

    return SUCCESS;
}
