#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>

#include <autocopt/autocopt.h>
#include <autocopt/emergency.h>

int main(int argc, char **argv) {
	struct autocopt *copt;
	int32_t ret;
	int fd;
	copt = autocopt_init(AUTOCOPT_DEFAULT_DEV);
	if (copt == NULL) {
		goto main_error0;
	}
	ret = autocopt_reset(copt);
	if (ret < 0) {
		goto main_error1;
	}
	/**
	 * No Application only reset
	 */
	if (argc < 2) {
		autocopt_deinit(copt);
		return EXIT_SUCCESS;
	}
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		goto main_error1;
	}
	ret = autocopt_copyAppToMem(0x3f000000UL, fd);
	if (ret < 0) {
		goto main_error2;
	}
	ret = close(fd);
	if (ret < 0) {
		perror("close");
		goto main_error1;
	}
	ret = autocopt_setEntrypoint(copt, 0x3f000001UL);
	if (ret < 0) {
		goto main_error1;
	}
	ret = autocopt_reset(copt);
	if (ret < 0) {
		goto main_error1;
	}
	autocopt_deinit(copt);
	return EXIT_SUCCESS;
main_error2:
	close(fd);
main_error1:
	autocopt_deinit(copt);
main_error0:
	return EXIT_FAILURE;
}
