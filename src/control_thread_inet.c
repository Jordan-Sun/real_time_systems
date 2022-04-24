/*
 * control_thread_inet.c
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

#include "control_thread_inet.h"
#include "socket_inet.h"
#include "sensor_packet.h"
#include "motor_packet.h"
#include "error.h"

#include <sys/epoll.h>
#include <string.h>

static int light_on = 0;
static int indoor_ready = 0;
static int outdoor_ready = 0;
static unsigned int seq = 0;

/*
 * Prints the usage message.
 */
void usage_msg(char *program)
{
    printf("Usage: %s PORT_NUM\n", program);
    printf("%s MIN_VALUE\n", MIN_CMD);
    printf("%s MAX_VALUE\n", MAX_CMD);
    printf("%s MOTOR_PATH\n", MOTOR_CMD);
    printf("%s LIGHT_PATH\n", LIGHT_CMD);
    printf("Reads data from indoor and outdoor ambient light sensors, process the data and control motor and light through sockets to maintain a light level between MIN_VALUE (default %d) lux and MAX_VALUE (default %d) lux.\n", DEFAULT_MIN, DEFAULT_MAX);
}

/*
 * Reads from the socket and store the visible light in data if
 * not NULL, and print the log to stdout.
 */
int use_sensor(int fd, int *data)
{
    sensor_packet_t packet;
    struct timespec recv_timestamp, diff_timestamp;

    if (recv(fd, &packet, sizeof(sensor_packet_t), MSG_WAITALL) != sizeof(sensor_packet_t))
    {
        return -ERR_RECV;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &recv_timestamp) < 0)
    {
        return -ERR_TIME;
    }

    diff_timestamp.tv_sec = recv_timestamp.tv_sec - packet.timestamp.tv_sec;
    diff_timestamp.tv_nsec = recv_timestamp.tv_nsec - packet.timestamp.tv_nsec;
    if (diff_timestamp.tv_nsec < 0)
    {
        diff_timestamp.tv_sec -= 1;
        diff_timestamp.tv_nsec += 1000000000;
    }

    if (data)
    {
        *data = packet.visible;
    }

    // printf("%d: visible=%dlux\tinfrared=%dlux\tfull=%dlux\tseq=%d\ttime=%ld.%ld\n", fd, packet.visible, packet.infrared, packet.full, packet.sequence, diff_timestamp.tv_sec, diff_timestamp.tv_nsec);

    return SUCCESS;
}

/*
 * Updates.
 */
int update(int motor_fd, int light_fd, int indoor, int outdoor, int min, int max)
{
    motor_packet_t packet;
    
    if (!(indoor_ready && outdoor_ready))
    {
        return SUCCESS;
    }

    if (indoor < min)
    {
        if (outdoor > min)
        {
            if (motor_fd)
            {
                printf("Rasing curtain.\n");
                if (clock_gettime(CLOCK_MONOTONIC, &packet.timestamp) < 0)
                {
                    return -ERR_TIME;
                }
                packet.sequence = seq++;
                packet.direction = MOTOR_CW;
                packet.turns = MOTOR_TURN;
                if (send(motor_fd, &packet, sizeof(motor_packet_t), 0) < 0)
                {
                    return -ERR_SEND;
                }
            }
            else
            {
                printf("Not connected to motor.\n");
            }
        }
        
        if (light_on)
        {
            printf("Too dim inside.\n");
        }
        else
        {
            light_on = 1;
            printf("Turning light on.\n");
            if (!light_fd)
            {
                printf("Not connected to light.\n");
            }
        }
    }
    else if (indoor > max)
    {
        if (light_on)
        {
            light_on = 0;
            printf("Turning light off.\n");
            if (!light_fd)
            {
                printf("Not connected to light.\n");
            }
        }
        else if (outdoor <= max)
        {
            if (motor_fd)
            {
                printf("Lowering curtain.\n");
                if (clock_gettime(CLOCK_MONOTONIC, &packet.timestamp) < 0)
                {
                    return -ERR_TIME;
                }
                packet.sequence = seq++;
                packet.direction = MOTOR_CCW;
                packet.turns = MOTOR_TURN;
                if (send(motor_fd, &packet, sizeof(motor_packet_t), 0) < 0)
                {
                    return -ERR_SEND;
                }
            }
            else
            {
                printf("Not connected to motor.\n");
            }
        }
    }
    else
    {
        printf("Stopping curtain.\n");
        if (clock_gettime(CLOCK_MONOTONIC, &packet.timestamp) < 0)
        {
            return -ERR_TIME;
        }
        packet.sequence = seq++;
        packet.direction = MOTOR_CW;
        packet.turns = MOTOR_STOP;
        if (send(motor_fd, &packet, sizeof(motor_packet_t), 0) < 0)
        {
            return -ERR_SEND;
        }
    }

    indoor_ready = 0;
    outdoor_ready = 0;
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    int conn_fd, data_fd, epoll_fd, indoor_fd, outdoor_fd, motor_fd, light_fd;
    int indoor, outdoor, min, max;
    char *command;
    char *host_tmp;
    char *serv_tmp;
    int indoor_tmp, outdoor_tmp, len, val_tmp;
    struct epoll_event ev, events[MAX_EVENTS];
    int nfds;
    struct sockaddr addr;
    socklen_t addr_len;

    if (argc != EXPECTED_ARGC)
    {
        usage_msg(argv[PROGRAM_NAME]);
        return -ERR_NUM_ARGC;
    }

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
    val_tmp = 0;

    conn_fd = init_socket(NULL, atoi(argv[PORT_NUM]), SOCK_BACKLOG);
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
    ev.data.fd = conn_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev) < SUCCESS)
    {
        perror("Failed to add connection socket to epoll");
        return -ERR_EPCTL;
    }

    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) < SUCCESS)
    {
        perror("Failed to add stdin to epoll");
        return -ERR_EPCTL;
    }

    for (;;)
    {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, EPOLL_TIMEOUT);

        if (nfds < 0)
        {
            perror("Failed to wait for epoll");
            return -ERR_EPWAIT;
        }
        else if (nfds == 0)
        {
            /* timeout behavior */
        }
        else
        {
            for (int i = 0; i < nfds; ++i)
            {
                if (events[i].events & EPOLLIN)
                {
                    /* input */
                    if (events[i].data.fd == STDIN_FILENO)
                    {
                        len = scanf("%ms", &command);
                        if (len > 0)
                        {
                            if (strcmp(command, MIN_CMD) == 0)
                            {
                                if(scanf("%d", &val_tmp))
                                {
                                    min = val_tmp;
                                    printf("Updated minimum to %d.\n", min);
                                }
                                else
                                {
                                    printf("%s MIN_VALUE\n", MIN_CMD);
                                }
                            }
                            else if (strcmp(command, MAX_CMD) == 0)
                            {
                                if(scanf("%d", &val_tmp))
                                {
                                    max = val_tmp;
                                    printf("Updated maximum to %d.\n", max);
                                }
                                else
                                {
                                    printf("%s MAX_VALUE\n", MAX_CMD);
                                }
                            }
                            else if (strcmp(command, MOTOR_CMD) == 0)
                            {
                                len = scanf("%ms %ms", &host_tmp, &serv_tmp);
                                if (len == 2)
                                {
                                    motor_fd = conn_socket(host_tmp, atoi(serv_tmp));
                                    if (motor_fd < SUCCESS)
                                    {
                                        motor_fd = 0;
                                    }
                                    else
                                    {
                                        printf("Connected to motor %d.\n", motor_fd);
                                    }
                                    free(host_tmp);
                                    free(serv_tmp);
                                }
                                else if (len == 1)
                                {
                                    free(host_tmp);
                                    printf("%s HOST PORT\n", MOTOR_CMD);
                                }
                                else
                                {
                                    printf("%s HOST PORT\n", MOTOR_CMD);
                                }
                            }
                            else if (strcmp(command, LIGHT_CMD) == 0)
                            {
                                len = scanf("%ms %ms", &host_tmp, &serv_tmp);
                                if (len == 2)
                                {
                                    light_fd = conn_socket(host_tmp, atoi(serv_tmp));
                                    if (light_fd < SUCCESS)
                                    {
                                        light_fd = 0;
                                    }
                                    else
                                    {
                                        printf("Connected to light %d.\n", light_fd);
                                    }
                                    free(host_tmp);
                                    free(serv_tmp);
                                }
                                else if (len == 1)
                                {
                                    free(host_tmp);
                                    printf("%s HOST PORT\n", LIGHT_CMD);
                                }
                                else
                                {
                                    printf("%s HOST PORT\n", LIGHT_CMD);
                                }
                            }
                            else
                            {
                                printf("Unknown command %s.\n", command);
                            }
                            free(command);
                        }
                        else
                        {
                            printf("%s MIN_VALUE\n", MIN_CMD);
                            printf("%s MAX_VALUE\n", MAX_CMD);
                            printf("%s HOST PORT\n", MOTOR_CMD);
                            printf("%s HOST PORT\n", LIGHT_CMD);
                            printf("%s HOST PORT\n", INDOOR_CMD);
                            printf("%s HOST PORT\n", OUTDOOR_CMD);
                        }
                    }
                    else if (events[i].data.fd == conn_fd)
                    {
                        data_fd = accept(conn_fd, (struct sockaddr *)&addr, &addr_len);
                        for (;;)
                        {
                            printf("Indoor or outdoor sensor? (%s/%s):", INDOOR_SYM, OUTDOOR_SYM);
                            fflush(stdout);
                            len = scanf("%ms", &command);
                            if (len > 0)
                            {
                                if (strcmp(command, INDOOR_SYM) == 0)
                                {
                                    indoor_fd = data_fd;
                                    break;
                                }
                                else if (strcmp(command, OUTDOOR_SYM) == 0)
                                {
                                    outdoor_fd = data_fd;
                                    break;
                                }
                                free(command);
                            }
                        }
                        ev.events = EPOLLIN | EPOLLRDHUP;
                        ev.data.fd = data_fd;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, data_fd, &ev) < SUCCESS)
                        {
                            perror("Failed to add data socket to epoll");
                            return -ERR_EPCTL;
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
                            indoor_ready = 1;
                            if (update(motor_fd, light_fd, indoor, outdoor, min, max))
                            {
                                printf("Failed to update.\n");
                            }
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
                            outdoor_ready = 1;
                            if (update(motor_fd, light_fd, indoor, outdoor, min, max))
                            {
                                printf("Failed to update.\n");
                            }
                        }
                    }
                }
                if (events[i].events & EPOLLRDHUP)
                {
                    /* disconnect */
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
                        ev.events = EPOLLIN | EPOLLRDHUP;
                        ev.data.fd = indoor_fd;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, indoor_fd, &ev) < SUCCESS)
                        {
                            perror("Failed to remove indoor socket from epoll");
                            return -ERR_EPCTL;
                        }
                        indoor_fd = 0;
                    }
                    else if (events[i].data.fd == outdoor_fd)
                    {
                        printf("Outdoor socket disconnected.\n");
                        ev.events = EPOLLIN | EPOLLRDHUP;
                        ev.data.fd = outdoor_fd;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, outdoor_fd, &ev) < SUCCESS)
                        {
                            perror("Failed to remove outdoor socket from epoll");
                            return -ERR_EPCTL;
                        }
                        outdoor_fd = 0;
                    }
                }
            }
        }
    }

    close(conn_fd);

    return SUCCESS;
}
