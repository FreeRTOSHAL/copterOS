#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/autocopt.h>

#define DEVICE "/dev/copt0"
int main(int argc, char **argv) {
	int fd;
	int ret;
	struct autocopt_msg msg = {
		.type = AUTOCOPT_TYPE_ACT,
	};
	memset(msg.data, 42, 31 * sizeof(char));
	fd = open(DEVICE, O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	printf("Ping\n");
	ret = write(fd, &msg, sizeof(struct autocopt_msg));
	if (ret < 0) {
		perror("write");
		return EXIT_FAILURE;
	}
	ret = read(fd, &msg, sizeof(struct autocopt_msg));
	if (ret < 0) {
		perror("read");
		return EXIT_FAILURE;
	}
	printf("Pong\n");
	ret = close(fd);
	if (ret < 0) {
		perror("close");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
