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
}
struct lc *lc_init(struct motor *motor) {
	struct lc *lc = &lc0;
	memset(lc, 0, sizeof(struct lc));
	lc->motor = motor;
#ifdef CONFIG_MACH_VF610
	irq_clear(3);
	irq_setPrio(3, 0xFF);
	irq_enable(3);
	irq_clear(0);
	irq_setPrio(0, 0xFF);
	irq_enable(0);
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
	for(;;); /* Block in Intterrupt */
}
#if 0
uint32_t *AIRCR = (uint32_t *) 0xE000ED0C;
void (*bootloader)() = (void *) 0x1f03ec01;
void cpu2cpu_int0_isr(void) {
	/* jump Bootloader */
	irq_clear(0);
	lc_shutdown(&lc0);
	*AIRCR |= (0x1 << 1);
	bootloader();
	for(;;); /* Never reach */
}
#endif
/* 
 * For Jump to Bootlaoder out of ISR, we use this Fake Stack
 * The local Reset Register and Externel Reset Register will not work on VF610 
 * Also the Exeption Reset Register
 */
uint32_t  booloaderFakeStack[] = {
	[0] = 0x42424242, /* R0 */
	[1] = 0x42424242, /* R1 */
	[2] = 0x42424242, /* R2 */
	[3] = 0x42424242, /* R3 */
	[4] = 0x42424242, /* R12 */
	[5] = 0x1f03ec01, /* LR Register set to Bootloader Entry Point */
	[6] = 0x1f03ec01, /* Bootloader PC */
	[7] = 0x01000000, /* XPSR */
};
__attribute__((naked)) void cpu2cpu_int0_isr(void) {
	asm volatile (
		"cpsid i" "\n"
		"cpsid f" "\n"
		"push {r0, r1, r2, r3, r4, lr}"
	);
	/* Shutdown all PWM */
	irq_clear(0);
	lc_shutdown(&lc0);
	/* jump Bootloader */
	asm volatile (
		"pop {r0, r1, r2, r3, r4, lr}" "\n"
		"ldr r2, =booloaderFakeStack" "\n"
		"msr psp, r2" "\n"
		"isb" "\n"
		"msr msp, r2" "\n"
		"isb" "\n"
		"bx lr" "\n"
	);
}
#endif
