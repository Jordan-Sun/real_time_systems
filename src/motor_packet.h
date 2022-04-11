/*
 * motor_packet.h
 *
 * The data packet sent from the control thread to the motor thread.
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

#ifndef MOTOR_PACKET_H
#define MOTOR_PACKET_H

#include <time.h>

typedef struct motor_packet
{
    /* Metadata */
    struct timespec timestamp;
    unsigned int sequence;
    /* Data */
    int direction;
    /*
        when turns > 0, the motor will rotate turns rounds,
        while turns == -1 turns == 0, the motor will stop immediately
    */
    int turns;
} motor_packet_t;

#endif /* MOTOR_PACKET_H */
