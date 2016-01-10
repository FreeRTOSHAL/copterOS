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


#include <linux/autocopt.h>

#define DEVICE "/dev/copt0"

void reset() {
	int fd;
	int ret;
	fd = open(DEVICE, O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	printf("Send Reset\n");
	ret = ioctl(fd, AUTOCOPT_IOCTL_BOOTLOADER, NULL);
	if (ret < 0) {
		perror("ioctl");
		exit(EXIT_FAILURE);
	}
	ret = close(fd);
	if (ret < 0) {
		perror("close");
		exit(EXIT_FAILURE);
	}
}
void copyAppToMem(uint32_t pdest, int srcFd) {
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
		exit(EXIT_FAILURE);
	}
	size = lseek(srcFd, 0, SEEK_END);
	lseek(srcFd, 0, SEEK_SET);

	mem = (uint8_t *) mmap(0, size + pdestoff, PROT_READ|PROT_WRITE, MAP_SHARED, fd, pdestaligned);
	if (mem == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	basemem = mem;
	do {
		ret = read(srcFd, buf, 4096);
		if (ret > 0) {
			memcpy(mem, buf, ret);
			mem+=ret;
		}
	} while(ret > 0);

	ret = munmap(basemem, size + pdestoff);
	if (ret < 0) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}

	ret = close(fd);
}

void setEntrypoint(uint32_t entry) {
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
		exit(EXIT_FAILURE);
	}

	mem = (uint8_t *) mmap(0, pdestoff, PROT_READ|PROT_WRITE, MAP_SHARED, fd, pdestaligned);
	if (mem == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	{
		uint32_t *p = (uint32_t *) (mem + pdestoff);
		*p = entry;
	}

	ret = munmap(mem, pdestoff);
	if (ret < 0) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}

	ret = close(fd);
}

int main(int argc, char **argv) {
	int ret;
	int fd;
	reset();
	if (argc < 2) {
		return EXIT_SUCCESS;
	}
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	copyAppToMem(0x3f000000UL, fd);
	ret = close(fd);
	if (ret < 0) {
		perror("close");
		exit(EXIT_FAILURE);
	}
	setEntrypoint(0x3f000001UL);
	reset();
	return EXIT_SUCCESS;
}
