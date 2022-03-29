/*
 * socket_inet.c
 *
 * Send a data packet to a inet (remote) socket.
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

#include "socket_inet.h"

#include <string.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

int init_socket(const char *host, unsigned int port, int backlog)
{
    /* File descriptor for the socket */
    int fd;
    /* Socket address name */
    struct sockaddr_in name;
    char hostname[NI_MAXHOST];

    /* Create a socket for inet connection */
    fd = socket(AF_INET, SOCK_STREAM, SOCK_PROTOCAL);
    if (fd < SUCCESS)
    {
        perror("Failed to open socket");
        return -ERR_SOCKET;
    }

    memset(&name, 0, sizeof(struct sockaddr_in));
    name.sin_family = AF_INET;
    if (host)
    {
        if (inet_aton(host, &(name.sin_addr)))
        {
            perror("Failed to resolve hostname");
            return -ERR_HOSTNAME;
        }
    }
    else
    {
        name.sin_addr.s_addr = INADDR_ANY;
    }
    name.sin_port = htons(port);

    /* Bind socket */
    if (bind(fd, (const struct sockaddr *)&name,
                sizeof(struct sockaddr_in)) < SUCCESS)
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

    
    /* Get hostname */
    if (gethostname(hostname, sizeof(hostname)) < 0)
    {
        perror("Failed to get hostname");
        return -ERR_HOSTNAME;
    }
    printf("Listening at %s on port %u.\n", hostname, port);

    return fd;
}

int conn_socket(const char *host, unsigned int port)
{
    /* File descriptor for the socket */
    int fd;
    /* Socket address name */
    struct sockaddr_in name;

    /* Create a socket for inet connection */
    fd = socket(AF_INET, SOCK_STREAM, SOCK_PROTOCAL);
    if (fd < SUCCESS)
    {
        perror("Failed to open socket");
        return -ERR_SOCKET;
    }

    memset(&name, 0, sizeof(struct sockaddr_in));
    name.sin_family = AF_INET;
    if (inet_aton(host, &(name.sin_addr)))
    {
        perror("Failed to resolve hostname");
        return -ERR_HOSTNAME;
    }
    name.sin_port = htons(port);

    /* Connect to socket */
    if (connect(fd, (const struct sockaddr *)&name,
                sizeof(struct sockaddr_in)) < SUCCESS)
    {
        perror("Failed to connect to socket");
        return -ERR_CONNECT;
    }

    return fd;
}
