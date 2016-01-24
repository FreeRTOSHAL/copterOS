#ifndef LINUX_COMMANDER_H_
#define LINUX_COMMANDER_H_
struct linuxComm *linuxComm_init(struct lc *lc);
int32_t linuxComm_deinit();
void linuxComm_selectThrust();
void linuxComm_selectRTY();
#endif
