#include <FreeRTOS.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <motor_ctrl.h>
#include <timer.h>
#include <pwm.h>

struct motor_unit {
	struct pwm *pwm;
	bool enable;
	uint32_t value;
};

struct motor {
	bool init;
	struct motor_unit unit[MOTOR_MAX_CHANNELS];
};
static struct motor m;
struct motor *motor_init() {
	struct motor *motor = &m;
	if (motor->init) {
		return motor;
	}
	memset(motor, 0, sizeof(struct motor));
	motor->init = true;
	return motor;
}
int32_t motor_deinit(struct motor *motor) {
	return 0;
}
int32_t motor_set(struct motor *motor, uint32_t id, uint32_t value) {
	if (id > MOTOR_MAX_CHANNELS && !motor->unit[id].enable) {
		return -1;
	}
	motor->unit[id].value = value;
	return pwm_setDutyCycle(motor->unit[id].pwm, value);
}
int32_t motor_enable(struct motor *motor, uint32_t id) {
	int32_t ret;
	int index = 0;
	for (index = 0; index < MOTOR_MAX_CHANNELS; index++) {
		if(motor->unit[index].pwm == NULL) {
			break;
		}
	}
	if (index == MOTOR_MAX_CHANNELS) {
		return -1;
	}
	motor->unit[index].enable = true;
	motor->unit[index].pwm = pwm_init(id); 
	ret = pwm_setPeriod(motor->unit[index].pwm, 20000);
	if (ret < 0) {
		return ret;
	}
	return motor_set(motor, index, motor->unit[index].value);
}
int32_t motor_disable(struct motor *motor, uint32_t id) {
	if (id > MOTOR_MAX_CHANNELS) {
		return -1;
	}
	motor->unit[id].enable = false;
	return motor_set(motor, id, 0);
}
