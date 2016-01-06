#include <FreeRTOS.h>
#include <task.h>
#include <linux_client.h>
#include <stdio.h>
#include <vector.h>
#include <motor_ctrl.h>
#include <string.h>
#include <irq.h>

/* TODO Move to config */
#define MOTOR_PIN1 3
#define MOTOR_PIN2 6
#define MOTOR_PIN3 2
#define MOTOR_PIN4 0

struct lc {
	struct motor *motor;
};
struct lc lc0;
static void lc_shutdown(struct lc *lc) {
	vTaskSuspendAll(); /* Stop all Tasks */
	taskDISABLE_INTERRUPTS(); /* Disable all Interrupts */
	motor_set(lc->motor, MOTOR_PIN1, 0);
	motor_set(lc->motor, MOTOR_PIN2, 0);
	motor_set(lc->motor, MOTOR_PIN3, 0);
	motor_set(lc->motor, MOTOR_PIN4, 0);
	for(;;); /* Block in Intterrupt */
}
struct lc *lc_init(struct motor *motor) {
	struct lc *lc = &lc0;
	memset(lc, 0, sizeof(struct lc));
	lc->motor = motor;
#ifdef CONFIG_MACH_VF610
	irq_clear(3);
	irq_setPrio(3, 0xFF);
	irq_enable(3);
#endif
	return lc;
}
int32_t lc_deinit(struct lc *lc) {
	vPortFree(lc);
	return 0;
}
#ifdef CONFIG_MACH_VF610
void cpu2cpu_int3_isr(void) {
	/* Emergency Shutdown */
	irq_clear(3);
	lc_shutdown(&lc0);
}
void cpu2cpu_int0_isr(void) {
	/* jump Bootloader */
}
#endif
