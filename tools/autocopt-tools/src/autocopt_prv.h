#ifndef AUTOCOPT_PRV_H_
#define AUTOCOPT_PRV_H_
#include <signal.h>
struct autocopt {
	char *dev;
	int fd;
	struct sigaction oldact;
};
#endif
