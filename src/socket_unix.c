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
#include "blocking_io.h"

int init_socket(const char *sock, int backlog)
{
    /* File descriptor for the socket */
    unsigned int fd;
    /* Socket address name */
    struct sockaddr_un name;

    /* Create a socket for unix connection */
    fd = socket(AF_UNIX, SOCK_STREAM, SOCK_PROTOCAL);
    if (fd < SUCCESS)
    {
        perror("Failed to open socket");
        return -ERR_SOCKET;
    }

    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, sock, sizeof(name.sun_path) - 1);

    /* Bind socket */
    if (bind(fd, (const struct sockaddr *)&name,
                sizeof(struct sockaddr_un)) < SUCCESS)
    {
        perror("Failed to bind socket");
        return -ERR_BIND;
    }

    /* Listen to socket */
    if (listen(fd, backlog) < SUCCESS)
    {
        perror("Failed to listen to socket");
        return -ERR_LISTEN;
    }

    return fd;
}

int conn_socket(const char *sock)
{
    /* File descriptor for the socket */
    unsigned int fd;
    /* Socket address name */
    struct sockaddr_un name;

    /* Create a socket for unix connection */
    fd = socket(AF_UNIX, SOCK_STREAM, SOCK_PROTOCAL);
    if (fd < SUCCESS)
    {
        perror("Failed to open socket");
        return -ERR_SOCKET;
    }

    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, sock, sizeof(name.sun_path) - 1);

    /* Connect to socket */
    if (connect(fd, (const struct sockaddr *)&name,
                sizeof(struct sockaddr_un)) < SUCCESS)
    {
        perror("Failed to connect to socket");
        return -ERR_CONNECT;
    }

    return fd;
}

int send_packet(unsigned int fd, const sensor_packet_t *packet)
{
    return blocking_write(fd, packet, sizeof(sensor_packet_t));
}

int recv_packet(unsigned int fd, sensor_packet_t *packet)
{
    return blocking_read(fd, packet, sizeof(sensor_packet_t));
}