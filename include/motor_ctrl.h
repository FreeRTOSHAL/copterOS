#ifndef MOTOR_CTRL_
#define MOTOR_CTRL_
#include <timer.h>
#include <remote_control.h>
#define MOTOR_MAX_CHANNELS 8
#define MOTOR_MIN RC_MIN
#define MOTOR_MAX RC_MAX
struct motor;
struct motor *motor_init();
int32_t motor_deinit(struct motor *motor);
int32_t motor_set(struct motor *motor, uint32_t id, uint32_t value);
int32_t motor_enable(struct motor *motor, uint32_t id);
int32_t motor_disable(struct motor *motor, uint32_t id);
#endif
