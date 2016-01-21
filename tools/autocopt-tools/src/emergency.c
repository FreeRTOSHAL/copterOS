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
#include "autocopt_prv.h"
int32_t autocopt_shutdown(struct autocopt *copt) {
	int ret;
	ret = ioctl(copt->fd, AUTOCOPT_IOCTL_EMERGENCY_SHUTDOWN, NULL);
	if (ret < 0) {
		perror("ioctl");
		return -1;
	}
	return 0;
}
int32_t autocopt_reset(struct autocopt *copt) {
	int ret;
	ret = ioctl(copt->fd, AUTOCOPT_IOCTL_BOOTLOADER, NULL);
	if (ret < 0) {
		perror("ioctl");
		return -1;
	}
	return 0;
}
int32_t autocopt_setEntrypoint(struct autocopt *copt, uint32_t entry) {
	int ret;
	int fd;
	uint8_t *mem;
	long pagesize = sysconf(_SC_PAGE_SIZE);
	uint32_t pdestoff = 0x3f07fffc & (pagesize - 1);
	uint32_t pdestaligned = 0x3f07fffc & ~(pagesize - 1);
	printf("Set Entrypoint to 0x%X\n", entry);
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd < 0) {
		perror("open Failed");
		goto autocopt_setEntrypoint_error0;
	}

	mem = (uint8_t *) mmap(0, pdestoff, PROT_READ|PROT_WRITE, MAP_SHARED, fd, pdestaligned);
	if (mem == MAP_FAILED) {
		perror("mmap");
		goto autocopt_setEntrypoint_error1;
	}
	{
		uint32_t *p = (uint32_t *) (mem + pdestoff);
		*p = entry;
	}

	ret = munmap(mem, pdestoff);
	if (ret < 0) {
		perror("munmap");
		goto autocopt_setEntrypoint_error1;
	}

	close(fd);
	return 0;
autocopt_setEntrypoint_error1:
	close(fd);
autocopt_setEntrypoint_error0:
	return -1;
}
int32_t autocopt_copyAppToMem(uint32_t pdest, int srcFd) {
	int ret;
	int fd;
	uint8_t *mem;
	uint8_t *basemem;
	long pagesize = sysconf(_SC_PAGE_SIZE);
	uint32_t pdestoff = pdest & (pagesize - 1);
	uint32_t pdestaligned = pdest & ~(pagesize - 1);
	int32_t size;
	uint8_t buf[4096];
	printf("Copy application to 0x%X\n", pdest);
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd < 0) {
		perror("open Failed");
		goto autocopt_copyAppToMem_error0;
	}
	size = lseek(srcFd, 0, SEEK_END);
	lseek(srcFd, 0, SEEK_SET);

	mem = (uint8_t *) mmap(0, size + pdestoff, PROT_READ|PROT_WRITE, MAP_SHARED, fd, pdestaligned);
	if (mem == MAP_FAILED) {
		perror("mmap");
		goto autocopt_copyAppToMem_error1;
	}
	basemem = mem;
	do {
		ret = read(srcFd, buf, 4096);
		if (ret > 0) {
			memcpy(mem, buf, ret);
			mem+=ret;
		}
		if (ret < 0) {
			perror("read");
			goto autocopt_copyAppToMem_error2;
		}
	} while(ret > 0);

	ret = munmap(basemem, size + pdestoff);
	if (ret < 0) {
		perror("munmap");
		goto autocopt_copyAppToMem_error1;
	}

	close(fd);
	return 0;
autocopt_copyAppToMem_error2:
	munmap(basemem, size + pdestoff);
autocopt_copyAppToMem_error1:
	close(fd);
autocopt_copyAppToMem_error0:
	return -1;
}
