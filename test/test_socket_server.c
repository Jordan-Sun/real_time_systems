/*
 * test_socket_server.c
 *
 * Test server for socket.
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

#include "test_socket.h"
#include "../src/socket.h"
#include "../src/sensor_packet.h"
#include "../src/blocking_io.h"
#include <assert.h>

int main()
{
    int conn_fd, data_fd;
    unsigned int expected;
    struct timespec recv_timestamp;
    struct timespec diff_timestamp;
    sensor_packet_t packet;
    
    unlink(SOCK_PATH);

    conn_fd = init_socket(SOCK_PATH, SOCK_BACKLOG);
    assert(conn_fd >= 0);

    data_fd = accept(conn_fd, NULL, NULL);
    assert(data_fd >= 0);

    for (expected = 0; expected < PACK_COUNT; ++expected)
    {
        packet.timestamp.tv_nsec = -1;
        packet.timestamp.tv_sec = -1;
        packet.sequence = (unsigned int) -1;
        packet.full = -1;
        packet.infrared = -1;
        packet.visible = -1;
        assert(recv_packet(data_fd, &packet) == sizeof(sensor_packet_t));
        assert(clock_gettime(CLOCK_MONOTONIC, &recv_timestamp) == 0);
        assert(packet.timestamp.tv_sec != -1);
        assert(packet.timestamp.tv_nsec != -1);
        /* Read failure */
        assert(packet.sequence != (unsigned int) -1);
        /* Packet loss */
        assert(packet.sequence == expected);
        assert(packet.full != -1);
        assert(packet.infrared != -1);
        assert(packet.visible != -1);

        diff_timestamp.tv_sec = recv_timestamp.tv_sec - packet.timestamp.tv_sec;
        diff_timestamp.tv_nsec = recv_timestamp.tv_nsec - packet.timestamp.tv_nsec;
        if (diff_timestamp.tv_nsec < 0)
        {
            diff_timestamp.tv_sec -= 1;
            diff_timestamp.tv_nsec += 1000000000;
        }

        printf("visible=%dlux\tinfrared=%dlux\tfull=%dlux\tseq=%d\ttime=%ld.%ld\n", packet.visible, packet.infrared, packet.full, packet.sequence, diff_timestamp.tv_sec, diff_timestamp.tv_nsec);
    }

    close(conn_fd);
    unlink(SOCK_PATH);

    return 0;
}