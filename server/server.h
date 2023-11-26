
#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "vdrm_ioctl.h"

typedef struct {
    int server_fd;                // File descriptor for the server socket
    int new_socket;               // File descriptor for the client's socket
    struct sockaddr_in address;   // Server address
    pthread_mutex_t socket_mutex; // Mutex for thread-safe socket operations
} ServerParameters;
void cleanup_server_parameters(ServerParameters *params);

ServerParameters* initialize_server();
int send_ioctl_data(int socket, struct ioctl_data *data);
int receive_ioctl_data(int socket, struct ioctl_data *data);

#endif // SERVER_H
