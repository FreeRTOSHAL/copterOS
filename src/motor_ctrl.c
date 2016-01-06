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
	struct motor_unit unit[MOTOR_MAX_CHANNELS];
};
struct motor *motor_init() {
	struct motor *motor = pvPortMalloc(sizeof(struct motor));
	if (motor == NULL) {
		return NULL;
	}
	memset(motor, 0, sizeof(struct motor));
	return motor;
}
int32_t motor_deinit(struct motor *motor) {
	return 0;
}
int32_t motor_set(struct motor *motor, uint32_t id, uint32_t value) {
	if (id > MOTOR_MAX) {
		return -1;
	}
	motor->unit[id].value = value;
	return pwm_setDutyCycle(motor->unit[id].pwm, value);
}
int32_t motor_enable(struct motor *motor, uint32_t id) {
	int32_t ret;
	if (id > MOTOR_MAX) {
		return -1;
	}
	motor->unit[id].enable = true;
	motor->unit[id].pwm = pwm_init(id); 
	ret = pwm_setPeriod(motor->unit[id].pwm, 20000);
	if (ret < 0) {
		return ret;
	}
	return motor_set(motor, id, motor->unit[id].value);
}
int32_t motor_disable(struct motor *motor, uint32_t id) {
	if (id > MOTOR_MAX) {
		return -1;
	}
	motor->unit[id].enable = false;
	return motor_set(motor, id, 0);
}
