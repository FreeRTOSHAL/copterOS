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
#include <stdbool.h>
#include <errno.h>

#include <autocopt/autocopt.h>

int main(int argc, char **argv) {
	int32_t ret;
	struct autocopt *copt;
	
	struct autocopt_msg msg = {
		.type = AUTOCOPT_TYPE_ACT,
	};
	memset(msg.data, 42, 31 * sizeof(char));

	copt = autocopt_init(AUTOCOPT_DEFAULT_DEV);
	if (copt == NULL) {
		return EXIT_FAILURE;
	}
	do {
		ret = autocopt_recv(copt, &msg, 1);
		if (ret < 0) {
			if (errno != EINTR) {
				autocopt_deinit(copt);
				return EXIT_FAILURE;
			}

		}
	} while(ret > 0);
	autocopt_deinit(copt);
	return EXIT_SUCCESS;
}

