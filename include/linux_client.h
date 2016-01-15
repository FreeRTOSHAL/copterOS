#ifndef LC_H_
#define LC_H_
#include <system.h>

#define LC_MSG_SIZE 31

#define LC_TYPE_ACT 0x0
#define LC_TYPE_STARTUP 0x1
#define LC_TYPE_PID 0x2

#define LC_COUNT 0x3
#define LC_BUFFER_SIZE 8

struct lc_msg {
	uint8_t type;
	uint8_t data[LC_MSG_SIZE];
} PACKED;

struct lc;
struct lc *lc_init();
int32_t lc_registerCallback(struct lc *lc, uint8_t type, void (*callback)(struct lc *lc, struct lc_msg *msg));
int32_t lc_send(struct lc *lc, struct lc_msg *msg);
int32_t lc_sendAct(struct lc *lc);
int32_t lc_deinit(struct lc *lc);
#endif
