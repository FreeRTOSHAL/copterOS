#ifndef EMERGENCY_H_
#define EMERGENCY_H_
#include <stdint.h>
#include <stdbool.h>
#include <motor_ctrl.h>
struct emergency;
struct emergency *emergency_init(struct motor* motor);
int32_t emergency_deinit(struct emergency *emer);
void emergency_landing();
bool emergency_landingISR();
void emergency_shutdown();
#endif
