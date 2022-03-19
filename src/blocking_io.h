/*
 * blocking_io.h
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

#ifndef BLOCKING_IO_H
#define BLOCKING_IO_H

#include <unistd.h>

/* blocks until all the data is written or the write fails */
ssize_t blocking_write(int fd, const void *buf, size_t len);

/* blocks until all the data is read or the read fails */
ssize_t blocking_read(int fd, void *buf, size_t len);

#endif /* BLOCKING_IO_H */