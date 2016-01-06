#include <stdio.h>
#include <stdlib.h>
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
	fd = open(DEVICE, O_RDWR);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}
	printf("Send Emergency Shutdown\n");
	ret = ioctl(fd, AUTOCOPT_IOCTL_EMERGENCY_SHUTDOWN, NULL);
	if (ret < 0) {
		perror("ioctl");
		return EXIT_FAILURE;
	}
	ret = close(fd);
	if (ret < 0) {
		perror("close");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
