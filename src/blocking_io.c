/*
 * blocking_io.c
 *
 * Synchronous io helper methods.
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

#include "blocking_io.h"

/* blocks until all the data is written or the write fails */
ssize_t blocking_write(int fd, const void *buf, size_t len)
{
    size_t pos = 0;
    ssize_t ret = 0;
    /* write repeatedly until all data is written */
    while (pos < len)
    {
        ret = write(fd, buf + pos, len - pos);
        if (ret < 0)
            return ret;
        pos += ret;
    }
    return len;
}

/* blocks until all the data is read or the read fails */
ssize_t blocking_read(int fd, void *buf, size_t len)
{
    size_t pos = 0;
    ssize_t ret = 0;
    /* write repeatedly until all data is written */
    while (pos < len)
    {
        ret = read(fd, buf + pos, len - pos);
        if (ret < 0)
            return ret;
        pos += ret;
    }
    return len;
}