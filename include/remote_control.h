#ifndef REMOTE_CONTROL_H_
#define REMOTE_CONTROL_H_
#include <stdint.h>
#define RC_MAX_CHANNELS 8
#define RC_MIN 480 
#define RC_MAX 2200
#include <timer.h>
#include <capture.h>
struct rc;
struct rc *rc_init(struct timer *timer);
int32_t rc_setup(struct rc *rc, struct capture *capture);
uint32_t rc_get(struct rc *rc, uint32_t channel);
#endif
