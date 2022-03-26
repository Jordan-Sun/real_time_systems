/*
 * control_thread.c
 *
 * Control thread.
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

#include "control_thread.h"
#include "socket.h"
#include "sensor_packet.h"
// #include "motor_packet.h"
#include "error.h"

#include <sys/epoll.h>

/*
 * Prints the usage message.
 */
void usage_msg(char *program)
{
    printf("Usage: %s SOCKET_PATH\n", program);
    printf("%s MIN_VALUE\n", MIN_CMD);
    printf("%s MAX_VALUE\n", MAX_CMD);
    printf("Reads data from indoor and outdoor ambient light sensors , process the data and control motor and light through sockets to maintain a light level between MIN_VALUE (default %d) lux and MAX_VALUE (default %d) lux.\n", DEFAULT_MIN, DEFAULT_MAX);
}

/*
 * Reads from the socket and store the visible light in data if
 * not NULL, and print the log to stdout.
 */
int use_sensor(int fd, int *data)
{
    sensor_packet_t sensor_packet;
    struct timespec recv_timestamp, diff_timestamp;

    if (recv_packet(data_fd, &sensor_packet) != sizeof(sensor_packet_t))
    {
        return -ERR_RECV;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &recv_timestamp) < 0)
    {
        return -ERR_TIME;
    }

    diff_timestamp.tv_sec = recv_timestamp.tv_sec - sensor_packet.timestamp.tv_sec;
    diff_timestamp.tv_nsec = recv_timestamp.tv_nsec - sensor_packet.timestamp.tv_nsec;
    if (diff_timestamp.tv_nsec < 0)
    {
        diff_timestamp.tv_sec -= 1;
        diff_timestamp.tv_nsec += 1000000000;
    }

    if (data)
    {
        *data = sensor_packet.visible;
    }

    printf("visible=%dlux\tinfrared=%dlux\tfull=%dlux\tseq=%d\ttime=%ld.%ld\n", sensor_packet.visible, sensor_packet.infrared, sensor_packet.full, sensor_packet.sequence, diff_timestamp.tv_sec, diff_timestamp.tv_nsec);

    return SUCCESS;
}

int main(int argc, char *argv[])
{
    int conn_fd, data_fd, epoll_fd, indoor_fd, outdoor_fd, motor_fd, light_fd;
    int indoor, outdoor, min, max;
    int indoor_tmp, outdoor_tmp;
    char sensor_type;
    struct epoll_event ev, events[MAX_EVENTS];

    if (argc != EXPECTED_ARGC)
    {
        usage_msg(argv[PROGRAM_NAME]);
        return -ERR_NUM_ARGC;
    }
    
    unlink(SOCK_PATH);

    indoor_fd = 0;
    outdoor_fd = 0;
    motor_fd = 0;
    light_fd = 0;

    indoor = 0;
    indoor_tmp = 0;
    outdoor = 0;
    outdoor_tmp = 0;
    min = DEFAULT_MIN;
    max = DEFAULT_MAX;

    conn_fd = init_socket(SOCK_PATH, SOCK_BACKLOG);
    if (conn_fd < SUCCESS)
    {
        perror("Failed to connect to socket");
        return -ERR_SOCKET;
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd < SUCCESS)
    {
        perror("Failed to create epoll");
        return -ERR_EPOLL;
    }

    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) < SUCCESS)
    {
        perror("Failed to add stdin to epoll");
        return -ERR_EPCTL;
    }

    ev.events = EPOLLIN;
    ev.data.fd = conn_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fd, &ev) < SUCCESS)
    {
        perror("Failed to add connection socket to epoll");
        return -ERR_EPCTL;
    }

    for (;;)
    {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, EPOLL_TIMEOUT);

        if (nfds < 0)
        {
            perror("Failed to wait for epoll");
            return -ERR_EPWAIT;
        }
        else if (nfds == 0)
        {
            /* timeout behavior */
            if (indoor_fd && outdoor_fd)
            {
                /* use previous data */
                printf("Timeout, using previous data.\n");
            }
            /* no connection */
        }
        else
        {
            for (size_t i = 0; i < nfds; ++i)
            {
                if (events[i].events & EPOLLIN)
                {
                    /* input */
                    if (events[i].data.fd == STDIN_FILENO)
                    {
                        printf("Stdin disconnected.\n");
                        return -ERR_EPHUP;
                    }
                    else if (events[i].data.fd == conn_fd)
                    {
                        printf("Connection socket disconnected.\n");
                        return -ERR_EPHUP;
                    }
                    else if (events[i].data.fd == indoor_fd)
                    {
                        printf("Indoor socket disconnected.\n");
                        indoor_fd = 0;
                    }
                    else if (events[i].data.fd == outdoor_fd)
                    {
                        printf("Outdoor socket disconnected.\n");
                        outdoor_fd = 0;
                    }
                }
                if (events[i].events & EPOLLRDHUP)
                {
                    /* disconnect */
                    if (events[i].data.fd == STDIN_FILENO)
                    {
                    }
                    else if (events[i].data.fd == conn_fd)
                    {
                        data_fd = accept(conn_fd, (struct sockaddr *)&addr, &addr_len);
                        for (;;)
                        {
                            printf("Indoor or outdoor sensor? (%c/%c):", INDOOR_SYM, OUTDOOR_SYM);
                            fflush(stdout);
                            scanf("%c", &sensor_type);
                            if (sensor_type == INDOOR_SYM)
                            {
                                indoor_fd = data_fd;
                                break;
                            }
                            else if (sensor_type == OUTDOOR_SYM)
                            {
                                outdoor_fd = data_fd;
                                break;
                            }
                        }
                    }
                    else if (events[i].data.fd == indoor_fd)
                    {
                        use_sensor(indoor_fd, &indoor_tmp);
                        if (indoor_tmp < SUCCESS)
                        {
                            printf("Failed to read from indoor sensor.\n");
                        }
                        else
                        {
                            indoor = indoor_tmp;
                        }
                    }
                    else if (events[i].data.fd == outdoor_fd)
                    {
                        use_sensor(outdoor_fd, &outdoor_tmp);
                        if (outdoor_tmp < SUCCESS)
                        {
                            printf("Failed to read from outdoor sensor.\n");
                        }
                        else
                        {
                            outdoor = outdoor_tmp;
                        }
                    }
                }
            }
        }
    }

    close(conn_fd);
    unlink(SOCK_PATH);

    return SUCCESS;
}