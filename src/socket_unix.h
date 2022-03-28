/*
 * socket.h
 *
 * Send a data packet to a socket depending on the implementation.
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

#ifndef SOCKET_H
#define SOCKET_H

#include "sensor_packet.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

/* Socket protocal */
#define SOCK_PROTOCAL 0

int init_socket(const char *sock, int backlog);

int conn_socket(const char *sock);

int send_packet(unsigned int fd, const sensor_packet_t *packet);

int recv_packet(unsigned int fd, sensor_packet_t *packet);

#endif /* SOCKET_H */
