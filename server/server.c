#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "server.h"
#include "vdrm_ioctl.h"

#define PORT 8080

void cleanup_server_parameters(ServerParameters *params) {
    if (params->server_fd != -1) {
        close(params->server_fd);
    }
    if (params->new_socket != -1) {
        close(params->new_socket);
    }
    pthread_mutex_destroy(&(params->socket_mutex));
    free(params);
}

/**
 * initialize_server - Initializes a TCP server and returns its parameters.
 *
 * This function sets up a TCP server socket, binds it to the specified
 * port on the host machine, and initializes the server parameters. 
 * It sets the socket options to allow reuse of the address and port 
 * (useful in case the server restarts).
 * 
 * Return: A ServerParameters structure containing the initialized server parameters.
 *         In case of failure, server_fd in the returned structure will be set to -1.
 */
ServerParameters* initialize_server() {
    ServerParameters *params = malloc(sizeof(ServerParameters));
    if (params == NULL) {
        perror("Failed to allocate memory for ServerParameters");
        return NULL;
    }

    int opt = 1;

    // Create a socket file descriptor
    params->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (params->server_fd == -1) {
        perror("socket failed");
        free(params);
        return NULL;
    }

    // Set socket options to allow reuse of address and port
    if (setsockopt(params->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(params->server_fd);
        free(params);
        return NULL;
    }

    // Set server address and port
    params->address.sin_family = AF_INET;         // IPv4 protocol
    params->address.sin_addr.s_addr = INADDR_ANY; // Any incoming interface
    params->address.sin_port = htons(PORT);       // Convert port number to network byte order

    // Bind the socket to the address and port
    if (bind(params->server_fd, (struct sockaddr *)&params->address, sizeof(params->address)) < 0) {
        perror("bind failed");
        close(params->server_fd);
        free(params);
        return NULL;
    }

    // Start listening for incoming connections
    if (listen(params->server_fd, 3) < 0) {
        perror("listen");
        close(params->server_fd);
        free(params);
        return NULL;
    }

    // Initialize the mutex
    pthread_mutex_init(&params->socket_mutex, NULL);

    return params;
}


/**
 * send_ioctl_data - Sends ioctl_data structure over a socket.
 * @socket: Socket file descriptor to send data over.
 * @data: Pointer to the ioctl_data structure to be sent.
 * 
 * Return: 0 on success, or -1 on error.
 */
int send_ioctl_data(int socket, struct ioctl_data *data) {
    ssize_t bytes_sent;
    size_t total_size = sizeof(struct ioctl_data);

    bytes_sent = write(socket, data, total_size);
    if (bytes_sent < 0) {
        perror("Failed to send data");
        return -1;
    }

    return 0;
}

/**
 * receive_ioctl_data - Receives ioctl_data structure from a socket.
 * @socket: Socket file descriptor to receive data from.
 * @data: Pointer to the ioctl_data structure to store received data.
 * 
 * Return: 0 on success, or -1 on error.
 */
int receive_ioctl_data(int socket, struct ioctl_data *data) {
    ssize_t bytes_received;
    size_t total_size = sizeof(struct ioctl_data);

    bytes_received = read(socket, data, total_size);
    if (bytes_received < 0) {
        perror("Failed to receive data");
        return -1;
    } else if (bytes_received == 0) {
        printf("Connection closed by the peer\n");
        return -1;
    }

    return 0;
}

