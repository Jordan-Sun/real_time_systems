/*
 * socket_unix.c
 *
 * Send a data packet to a unix (local) socket.
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

#include "socket.h"

int init_socket(const char *socket)
{
    /* Socket address name */
    struct sockaddr_un name;

    return SUCCESS;
}

int send_packet(sensor_packet_t *packet)
{
    return SUCCESS;
}