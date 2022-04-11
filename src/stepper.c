#include "stepper.h"

void usage(char* progname) {
    printf("usage: %s <ENABLE_NUM> <DIR_NUM> <PUL_NUM> <DIRECTION> <PORT_NUM>\n", progname);
}

int msleep(long tms)
{
    struct timespec ts;
    int ret;

    if (tms < 0){
        errno = EINVAL;
        return -1;
    }
    ts.tv_sec = tms / 1000;
    ts.tv_nsec = (tms % 1000) * 1000000;

    do {
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

    return ret;
}

void rotate(int rotations, int pul_fd, int ena_fd) {
    if (rotations > 1000 || rotations <= 0) {
        perror("rotations should be in (0, 100]\n");
        return;
    }
    write(ena_fd, LOW, 1);
    for (int i = 1; i <= rotations; i++) {
        write(pul_fd, HIGH, 1);
        msleep(INTERVAL);
        write(pul_fd, LOW, 1);
        msleep(INTERVAL);
        if (i % 5 == 0) {
            printf("already rotates %d/%d\n", i, rotations);
        }
    }
    write(ena_fd, HIGH, 1);
    printf("rotation finished.\n");
}

int islegal(char* target) {
    size_t len = strlen(target);
    if (len > 10u) {
        perror("input should be at most 10 characters.\n");
        return 0;
    }
    for (size_t i = 0; i < len; ++i) {
        if (i == 0 && target[i] == '-') {
            if (len <= 1) return 0;
            continue;
        }
        if (target[i] < '0' || target[i] > '9') {
            return 0;
        }
    }
    return 1;
}

int set_direction(int direction, int dir_fd) {
    if (direction != 0 && direction != 1) {
        perror("direction should be 0 or 1.\n");
        return 0;
    }
    char* dir = direction == 1 ? HIGH : LOW;
    int ret = write(dir_fd, dir, 1);
    if (ret < 0) {
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    return 1;
}

int export(char** argv) {
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/export");
        printf("error: %s\n", strerror(errno));
        return 0;
    }

    for (int i = ENABLE_NUM; i <= PUL_NUM; ++i) {
        if (write(fd, argv[i], 2) <= 0) {
            printf("Unable to write %s to /sys/class/gpio/export\n", argv[i]);
            printf("error: %s\n", strerror(errno));
            return 0;
        }
    }
    close(fd);
    return 1;
}

void unexport(char **argv) {
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/unexport");
        printf("error: %s\n", strerror(errno));
    }

    for (int i = ENABLE_NUM; i <= PUL_NUM; ++i) {
        if (write(fd, argv[i], 2) < 0) {
            printf("Unable to write %s to /sys/class/gpio/unexport\n", argv[i]);
            printf("error: %s\n", strerror(errno));
        }
    }
    close(fd);
}

void seq_rotate(int rotations, int pul_fd, int ena_fd) {
    int previous_working_state = working;
    if (rotations < 0) {
        if (previous_working_state == 0) {
            working = 1;
            pthread_mutex_unlock(&motor_mutex);
        }
    } else {
        if (previous_working_state == 1) {
            pthread_mutex_lock(&motor_mutex);
            working = 0;
        }
        if (rotations > 0) {
            rotate(rotations, pul_fd, ena_fd);
            if (working != previous_working_state) {
                working = previous_working_state;
                pthread_mutex_unlock(&motor_mutex);
            }   
        }
    }
}

void *workerFn(void *data) {
    printf("worker thread has been started.\n");
    while (1) {
        if (working < 0) {
            break;
        } else if (working == 0) {
            pthread_mutex_lock(&motor_mutex);
            pthread_mutex_unlock(&motor_mutex);
        } else {
            rotate(WORKER_ROUNDS, pulse_fd, enable_fd);
        }
    }
    printf("worker thread has been terminated.\n");
    return data;
}


// #IFDEF __DEBUG_STEPPER
int main(int argc, char **argv) {
    int rotations = 0, fd, ret;
    int epoll_fd = -1;
    int connection_socket, ready, need_quit = 0, direction = 0, dir_in;
    unsigned int port_number;
    char buf[BUFSZ];
    // used socket
    struct sockaddr_in local_socket, in_socket;
    socklen_t addr_len;
    char server_host_name[NI_MAXHOST];
    struct epoll_event ev, evlist[EVENT_SIZE];
    motor_packet_t in_packet;
    pthread_t worker;

    // usage
    if (argc != ARGS_NUMBER) {
        usage(argv[PROGRAM_NAME]);
        exit(EXIT_FAILURE);
    }

    // in case those gpio are opened by others.
    unexport(argv);

    if (!export(argv)) {
        perror("export failed!");
        exit(EXIT_FAILURE); 
    }

    printf("Start dir initialization.\n");

    //initialize direction
    for (int i = ENABLE_NUM; i <= PUL_NUM; ++i) {
        ret = sprintf(buf, DIR_FMT, argv[i]);
        if (ret < 0) {
            printf("Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("initializing %s\n", buf);
        fd = open(buf, O_WRONLY);
        if (fd == -1) {
            printf("error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        ret = write(fd, OUT, 3);
        if (ret < 0) {
            printf("Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        close(fd);
    }

    printf("End dir initialization.\n");
    printf("Start value initialization.\n");

    //initialize value
    for (int i = ENABLE_NUM; i < PUL_NUM; ++i) {
        ret = sprintf(buf, VALUE_FMT, argv[i]);
        if (ret < 0) {
            printf("Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("initializing %s\n", buf);
        fd = open(buf, O_WRONLY);
        if (fd == -1) {
            printf("error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        // TODO error handle
        if (i == DIR_NUM) {
            direction_fd = fd;
            if (strcmp(argv[DIRECTION], "+") == 0)
                ret = write(fd, HIGH, 1);
            else
                ret = write(fd, LOW, 1);
        } else {
            enable_fd = fd;
            // warning: the low used for ena is for working only
            ret = write(fd, HIGH, 1);
        }
        if (ret < 0) {
            printf("Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    
    printf("End value initialization.\n");
    printf("start pulse initialization.\n");

    ret = sprintf(buf, VALUE_FMT, argv[PUL_NUM]);
    if (ret < 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("pulse value is %s\n", buf);
    pulse_fd = open(buf, O_WRONLY);
    if (pulse_fd < 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    ret = write(pulse_fd, LOW, 1);
    if (ret < 0) {
        printf("Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("End pulse initialization.\n");

    // ----------------------------------------
    // convert port number to unsigned int.
    port_number = atoi(argv[PORT_NUMBER]);
    
    // create a socket for local connection.
    connection_socket = socket(AF_INET, SOCK_STREAM, SOCKET_PROTOCAL);
    // on error, -1 is returned.
    if (connection_socket < 0) {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Created connection socket %u.\n", connection_socket);

    // connected, get name info
    ret = gethostname(server_host_name, sizeof(server_host_name));
    // on error, -1 is returned.
    if (ret < 0) {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Hosting on %s on port %u.\n", server_host_name, port_number);

    // create the communications channel.
    memset(&local_socket, 0, sizeof(struct sockaddr_in));
    local_socket.sin_family = AF_INET;
    local_socket.sin_port = htons(port_number);
    local_socket.sin_addr.s_addr = INADDR_ANY;
    ret = bind(connection_socket, (const struct sockaddr *) &local_socket, sizeof(struct sockaddr_in));
    // on error, -1 is returned.
    if (ret < 0) {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    // start listening on connection socket.
    ret = listen(connection_socket, SOCKET_BACKLOG_SIZE);
    // on error, -1 is returned.
    if (ret < 0) {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
	printf("Start listening on connection socket %u.\n", connection_socket);

    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)  {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ev.data.fd = connection_socket;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_socket, &ev);
    if (ret < 0)  {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ev.data.fd = STDIN_FILENO;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    if (ret < 0)  {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ret = pthread_create(&worker, NULL, workerFn, NULL);
    if (ret != 0) {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ret = pthread_detach(worker);
    if (ret != 0) {
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&motor_mutex);

    printf("worker thread has been set.\n");


    printf("Waiting for command.\n");

    // listen command
    // TODO socket module
    // while loop for accept socket connections.
    do {
        //epoll will block until a connection is ready for I/O
        if ((ready = epoll_wait(epoll_fd, evlist, EVENT_SIZE, -1)) > 0) {
            if (ready < 0 && errno != EINTR) {
                perror("epoll_wait wrong.");
            }
            for (int i = 0; i < ready; i++) {
                // input is ready
                if (evlist[i].events & EPOLLIN) {
                    // new connection.
                    if (evlist[i].data.fd == connection_socket) {    
                        printf("Detected new connection\n");
                        if ((fd = accept(connection_socket, (struct sockaddr *)&in_socket, &addr_len)) < 0) {
                            printf("error: %s\n", strerror(errno));
                            continue;
                        }
                        printf("Incomming connection: %s port:%u\n", inet_ntoa(in_socket.sin_addr), in_socket.sin_port);
                        // register the new socket into epoll
                        ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
                        ev.data.fd = fd;
                        ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
                        if (ret < 0) {
                            printf("error: %s\n", strerror(errno));
                        }

                    // data from stdin
                    } else if (evlist[i].data.fd == STDIN_FILENO) {
                        ret = read(STDIN_FILENO, buf, BUFSZ);
                        if (ret < 0) {
                            printf("Error: %s\n", strerror(errno));
                            continue;
                        }
                        buf[ret-1] = '\0';
                        printf("input: %s\n", buf);
                        if (strcmp(buf, "q") == 0 || strcmp(buf, "quit") == 0) {
                            need_quit = 1;
                            break;                       
                        } else if (islegal(buf)) {
                            rotations = atoi(buf);
                            seq_rotate(rotations, pulse_fd, enable_fd);
                        } 

                    // data from client.
                    } else {
                        fd = evlist[i].data.fd;
                        printf("Get command from client\n");
                        ret = read(fd, &in_packet , sizeof(motor_packet_t));
                        if (ret < 0) {
                            printf("Error: %s\n", strerror(errno));
                            continue;
                        }
                        dir_in = in_packet.direction;
                        rotations = in_packet.turns;
                        printf("Receiving command to rotate motor %d pulses in %d direction\n", rotations, dir_in);
                        if (dir_in != direction) {
                            ret = set_direction(dir_in, direction_fd);
                            if (!ret) continue;
                            direction = dir_in;
                        }
                        seq_rotate(rotations, pulse_fd, enable_fd);
                    }
                // peer disconnected.
                } else if (evlist[i].events & (EPOLLRDHUP | EPOLLRDHUP)) {
                    fd = evlist[i].data.fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
            }
        }
    } while (!need_quit);

    close(connection_socket);
    close(pulse_fd);
    close(direction_fd);
    close(enable_fd);
    unexport(argv);

    return 0;

}
// #ENDIF

// #IFNDEF __DEBUG_STEPPER
// int main() {

// }
// #ENDIF