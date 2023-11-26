#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "vdrm_ioctl.h"
#include"server.h"

#define BUFFER_SIZE 16384+sizeof(struct ioctl_data) //2^14

const char path[] = "/dev/vtux";

int main() {
	int fd = open(path, O_RDWR);
	if (fd < 0) {
		printf("failed to open vtux controller, what: %s\n", strerror(-fd));
		return fd;
	}
	char buf[BUFFER_SIZE];
	memset(buf, 0, BUFFER_SIZE);
	struct ioctl_data *ioctl;
	ssize_t byteRead = 0;


    ServerParameters *params;

    // Initialize server
    params = initialize_server();
    if (params->server_fd == -1) {
        fprintf(stderr, "Failed to initialize server\n");
        exit(1);
    }

    printf("Waiting for new connection...\n");
    params->new_socket = accept(params->server_fd, (struct sockaddr *)&params->address, (socklen_t *)sizeof(params->address));
    if (params->new_socket < 0) {
        perror("accept");
        exit(1);
    }

	while (1) {
		printf("before read\n");
		byteRead = read(fd, buf, BUFFER_SIZE);
		printf("after read\n");
		if(byteRead < 0) {
			printf("failed to read from vtux, what: %ld\n", byteRead);
			continue;
		}
		if (!byteRead) {
			printf("read 0 bytes from vtux\n");
			continue;
		}
		if (byteRead < BUFFER_SIZE) {
			buf[byteRead] = '\0';
		}
		printf("buffer: %s\n", buf);
		ioctl = (struct ioctl_data*) buf;
		printf("ioctl command: 0x%x/ %u\n", ioctl->request, ioctl->request);
		printf("ioctl arg size: %d\n", ioctl->size);
		close(fd);
		break;
	}

    // Close the client socket
    close(params->new_socket);

    // Cleanup resources
    cleanup_server_parameters(params);

	return EXIT_SUCCESS;
}