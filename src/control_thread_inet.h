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

#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

/* Control command words */
#define MIN_CMD "min"
#define MAX_CMD "max"
#define MOTOR_CMD "motor"
#define LIGHT_CMD "light"
#define INDOOR_CMD "indoor"
#define OUTDOOR_CMD "outdoor"
/* Default threshold */
#define DEFAULT_MIN 128
#define DEFAULT_MAX 256
/* Motor movement */
#define MOTOR_CW 0
#define MOTOR_CCW 1
#define MOTOR_TURN -1
#define MOTOR_STOP 0
/* Timeout interval (ms) */
#define EPOLL_TIMEOUT -1

/* Argument indices */
enum argi
{
    /* Program name */
    PROGRAM_NAME,
    /* Port number */
    PORT_NUM,
    /* Expected number of arguments */
    EXPECTED_ARGC
};

#endif /* CONTROL_THREAD_H */
