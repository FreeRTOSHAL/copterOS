#include <stdbool.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "motors.h"
#include <motor_ctrl.h>

struct motorPerifDef_s {
	struct motor *motor;
	int32_t id[];
};

static struct motorPerifDef_s motor = {
	.id = {
		0,
		1,
		2,
		3,
	},
};

void motorsInit(const MotorPerifDef** motorMapSelect) {
	motor.motor = motor_init();
	CONFIG_ASSERT(motor.motor != NULL);
}
void motorsDeInit(const MotorPerifDef** motorMapSelect) {
	int ret;
	int i;
	for (i = 0; i < NBR_OF_MOTORS; i++) {
		ret = motor_set(motor.motor, motor.id[i], 0);
		CONFIG_ASSERT(ret >= 0);
		ret = motor_disable(motor.motor, motor.id[i]);
		CONFIG_ASSERT(ret >= 0);
	}
}
bool motorsTest(void) {
#ifdef CONFIG_MOTOR_TEST
	int ret;
	int i;
	for (i = 0; i < NBR_OF_MOTORS; i++) {
		ret = motor_set(motor.motor, motor.id[i], 1000);
		CONFIG_ASSERT(ret >= 0);
		vTaskDelay(M2T(500));
		ret = motor_set(motor.motor, motor.id[i], 1100);
		CONFIG_ASSERT(ret >= 0);
		vTaskDelay(M2T(1000));
		ret = motor_set(motor.motor, motor.id[i], 1000);
		CONFIG_ASSERT(ret >= 0);
	}
#endif
	return true;
}
void motorsSetRatio(uint32_t id, uint16_t ratio) {
	uint32_t value = ((((uint32_t) ratio) * ((CONFIG_MOTOR_ESC_MAX - (CONFIG_MOTOR_ESC_MIN))))  / UINT16_MAX) + (CONFIG_MOTOR_ESC_MIN); 
	/*printf("Set Motor: %lu to %lu\n", id, value);*/
	motor_set(motor.motor, motor.id[id], value);
}
int motorsGetRatio(uint32_t id) {
	/* TODO */
	return -1;
}
