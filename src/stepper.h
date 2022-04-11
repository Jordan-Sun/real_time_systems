#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <time.h>
#include "motor_packet.h"
// for unix
#include <sys/un.h>
// for remote
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
//for epoll
#include <sys/epoll.h>
// for thread
#include <pthread.h>


#define HIGH "1"
#define LOW "0"
#define OUT "out"
#define INTERVAL 3
#define BUFSZ 128 
#define VALUE_FMT "/sys/class/gpio/gpio%s/value"
#define DIR_FMT "/sys/class/gpio/gpio%s/direction"
#define EVENT_SIZE 4
#define SOCKET_PROTOCAL 0
#define SOCKET_BACKLOG_SIZE 255
#define WORKER_ROUNDS 10 

// #define __DEBUG_STEPPER 1

enum ARGS{
    PROGRAM_NAME,
    ENABLE_NUM,
    DIR_NUM,
    PUL_NUM,
    DIRECTION,
    PORT_NUMBER,
    ARGS_NUMBER
};

static volatile short working = 0; // 1 for working, 0 for stop, -1 for termination.
static pthread_mutex_t motor_mutex = PTHREAD_MUTEX_INITIALIZER;
static int enable_fd = -1, pulse_fd = -1, direction_fd = -1;

// /** initialize the config of stepper motor.
// * input should be the bcm number of corresponding gpio pins.
// * output will be the pulse fd if export successfully. 
// **/
// int motor_export(int enable, int direction, int pulse);

// /** free the gpio resource
//  * input should be the bcm number of corresponding gpio pins
//  * return 1 if unexport successfully, otherwise return 0.
//  **/
// int motor_unexport(int enable, int direction, int pulse);

// /** control how many pulses will be send to rotation in a given internal.
//  * input should contain the corresponding pulse fd.
//  * return -1 for error.
//  **/
// int motor_rotate(int rotations, int pul_fd, int interval_ms);

// /** control the direction of stepper.
//  * 1 for clock wise, -1 for counter clock wise
//  * return -1 for error.
//  **/
// int motor_direction(int cw, int direction_pin);