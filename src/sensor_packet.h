/*
 * sensor_packet.h
 *
 * The data packet sent by the sensor thread to the socket.
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

#ifndef SENSOR_PACKET_H
#define SENSOR_PACKET_H

#include <time.h>

typedef struct sensor_packet
{
	time_t timestamp;
	int full;
	int infrared;
	int visible;
} sensor_packet_t;

#endif /* SENSOR_PACKET_H */
