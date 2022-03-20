/*
 * test_socket_client.c
 *
 * Test client for socket.
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
    int fd;
    unsigned int seq;
    sensor_packet_t packet;

    fd = conn_socket(SOCK_PATH);
    assert(fd >= 0);

    for (seq = 0; seq < PACK_COUNT; ++seq)
    {
        packet.timestamp = time(NULL);
        packet.sequence = seq;
        packet.full = seq;
        packet.infrared = seq;
        packet.visible = seq;
        assert(send_packet(fd, &packet) == sizeof(sensor_packet_t));
    }

    close(fd);

    return 0;
}