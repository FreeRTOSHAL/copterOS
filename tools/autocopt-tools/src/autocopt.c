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
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <autocopt/autocopt.h>
#include <linux/autocopt.h>
#include "autocopt_prv.h"

#define AUTCOPT_TIMEOUT 1

void sig_stat(int s) {
	/* TODO */
}

struct autocopt *autocopt_init(char *dev) {
	struct autocopt *copt = calloc(sizeof(struct autocopt), 1);
	if (copt == NULL) {
		perror("calloc");
		goto autocopt_init_error0;
	}
	copt->dev = calloc(sizeof(char), strlen(dev));
	if (copt->dev == NULL) {
		perror("calloc");
		goto autocopt_init_error1;
	}
	strcpy(copt->dev, dev);
	copt->fd = open(copt->dev, O_RDWR);
	if (copt->fd < 0) {
		perror("open");
		goto autocopt_init_error2;
	}
	{
		struct sigaction act;
		int ret;
		memset(&act, 0, sizeof(struct sigaction));
		act.sa_handler = sig_stat;
		ret = sigaction(SIGALRM, &act, &copt->oldact);
		if (ret < 0) {
			perror("sigaction");
			goto autocopt_init_error3;
		}
	}
	return copt;
autocopt_init_error3:
	close(copt->fd);
autocopt_init_error2:
	free(copt->dev);
autocopt_init_error1:
	free(copt);
autocopt_init_error0:
	return NULL;
}
int32_t autocopt_deinit(struct autocopt *copt) {
	/**
	 * Reset Signal Handler
	 */
	{
		int ret;
		ret = sigaction(SIGALRM, &copt->oldact, NULL);
		if (ret < 0) {
			perror("sigaction");
		}
	}
	close(copt->fd);
	free(copt->dev);
	free(copt);
	return 0;
}
int32_t autocopt_send(struct autocopt *copt, struct autocopt_msg *msg, size_t len) {
	int ret;
	int i;
	alarm(AUTCOPT_TIMEOUT);
	/* TODO Support Multiply write in linux kernel */
	for (i = 0; i < len; i++) {
		ret = write(copt->fd, msg, sizeof(struct autocopt_msg));
		if (ret < 0) {
			if (errno != EINTR) {
				perror("write");
				return -1;
			}
			break;
		}
	}
	alarm(0);
	return i;
}

int32_t autocopt_recv(struct autocopt *copt, struct autocopt_msg *msg, size_t len) {
	int ret;
	int i;
	alarm(AUTCOPT_TIMEOUT);
	/* TODO Support Multiply read in linux kernel */
	for (i = 0; i < len; i++) {
		ret = read(copt->fd, msg, sizeof(struct autocopt_msg));
		if (ret < 0) {
			if (errno != EINTR) {
				perror("read");
				return -1;
			}
			break;
		}
	}
	alarm(0);
	return i;
}
int32_t autocopt_ping(struct autocopt *copt) {
	struct autocopt_msg msg =  {
		.type = AUTOCOPT_TYPE_ACT,
	};
	int32_t ret;
	memset(msg.data, 42, 31 * sizeof(char));
	ret = autocopt_send(copt, &msg, 1);
	if (ret != 1) {
		return -1;
	}
	ret = autocopt_recv(copt, &msg, 1);
	if (ret != 1) {
		return -1;
	}
	return 0;
}
int32_t autocopt_select(struct autocopt *copt, uint32_t mode) {
	struct autocopt_msg msg = {
		.type = AUTOCOPT_TYPE_SELECT,
		.data[0] = mode,
	};
	int32_t ret = autocopt_send(copt, &msg, 1);
	if (ret < 0) {
		return -1;
	}
	ret = autocopt_recv(copt, &msg, 1);
	if (ret < 0) {
		return -1;
	}
	if (msg.type != AUTOCOPT_TYPE_ACT) {
		return -1;
	}
	return 0;
}
int32_t autocopt_control(struct autocopt *copt, struct autocopt_control *ctl) {
	struct autocopt_msg msg = {
		.type = AUTOCOPT_TYPE_CONTROL,
	};
	int32_t ret;
	memcpy(msg.data, ctl, sizeof(struct autocopt_control));
	ret = autocopt_send(copt, &msg, 1);
	if (ret < 0) {
		return -1;
	}
	ret = autocopt_recv(copt, &msg, 1);
	if (ret < 0) {
		return -1;
	}
	if (msg.type != AUTOCOPT_TYPE_ACT) {
		return -1;
	}
	return 0;
}
int32_t autocopt_pid(struct autocopt *copt, struct autocopt_pidValues *values) {
	struct autocopt_msg msg = {
		.type = AUTOCOPT_TYPE_PID,
	};
	int32_t ret;
	memcpy(msg.data, values, sizeof(struct autocopt_pidValues));
	ret = autocopt_send(copt, &msg, 1);
	if (ret < 0) {
		return -1;
	}
	ret = autocopt_recv(copt, &msg, 1);
	if (ret < 0) {
		return -1;
	}
	if (msg.type != AUTOCOPT_TYPE_ACT) {
		return -1;
	}
	return 0;
}
