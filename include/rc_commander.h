#ifndef RC_COMMANDER_H_
#define RC_COMMANDER_H_
#include <remote_control.h>
struct rcComm;
struct rcComm *rcComm_init(struct rc *rc);
int32_t rcComm_deinit();
void rcComm_selectThrust();
void rcComm_selectRTY();
#endif
