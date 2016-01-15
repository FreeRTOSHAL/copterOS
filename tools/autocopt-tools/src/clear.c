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
#include <stdbool.h>
#include <signal.h>
static bool running = true;
/**
 * SIGINT Handler
 * Exit Program
 */
static void sig_handler(int signum) {
	switch (signum) {
		case SIGINT:
			running = false;
			break;
	}
}

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
	{
		struct sigaction act;
		int ret;
		memset(&act, 0, sizeof(struct sigaction));
		act.sa_handler = sig_handler;
		ret = sigaction(SIGINT, &act, NULL);
		if (ret < 0) {
			perror("sigaction");
			exit(EXIT_FAILURE);
		}
	}
	while (running) {
		ret = read(fd, &msg, sizeof(struct autocopt_msg));
		if (ret < 0) {
			perror("read");
			return EXIT_FAILURE;
		}
	}
	ret = close(fd);
	if (ret < 0) {
		perror("close");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

