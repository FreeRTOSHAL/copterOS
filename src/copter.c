#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <gpio.h>
#include <iomux.h>
#include <task.h>
#include <pwm.h>
#include <irq.h>
#include <uart.h>
#include <newlib_stub.h> 
#include <timer.h>
#include <motor_ctrl.h>
#include <remote_control.h>
#include <linux_client.h>
#include "csystem.h"
#include <mpu9250.h>
#include <spi.h>
#include <adc.h>
#include <tps65381.h>


#define RC_TIMER 3
#define RC_PERIOD 24000
#define RC_PIN1 5
#define RC_PIN2 4
#define RC_PIN3 3
#define RC_PIN4 2
#define RC_PIN5 7
#define RC_PIN6 6

#define EM_LED 8

#define MOTOR_TIMER 0
#define MOTOR_PERIOD 20000
#define MOTOR_PIN1 3
#define MOTOR_PIN2 6
#define MOTOR_PIN3 0
#define MOTOR_PIN4 2

/*
 * Create MPU, Gyro and Accel Device
 */
MPU9250_ADDDEV(mpu0, 1, 0, SPI_OPT_GPIO_DIS, 500000);

static struct gpio *gpio = NULL;

static struct gpio_pin *pinPTB17 = NULL;

int32_t initGPIO() {
	gpio = gpio_init();
	if (gpio == NULL) {
		return -1;
	}
	pinPTB17 = gpioPin_init(gpio, PTB17, GPIO_OUTPUT, GPIO_PULL_UP);
	if (pinPTB17 == NULL) {
		return -1;
	}
	return 0;
}

void vApplicationMallocFailedHook( void ) {
	CONFIG_ASSERT(0);
}

void vApplicationTickHook() {
}

void vApplicationStackOverflowHook() {
	CONFIG_ASSERT(0);
}

void vApplicationIdleHook( void ) {
}

void ledTask(void *data) {
	int32_t ret;
	bool up = true;
	uint64_t n = 10000;
	struct pwm *pwm = data;
	TickType_t waittime = 20;
	TickType_t lastWakeUpTime = xTaskGetTickCount();
	for(;;) {
		if (up) {
			if (n >= (20000 - 400)) {
				up = false;
			}
			n+=400;
		} else {
			if (n <= 400) {
				up = true;
			}
			n-=400;
		}
		ret = pwm_setDutyCycle(pwm, n);
		CONFIG_ASSERT(ret == 0);
		if (n == 0 || n == 20000) {
			waittime = 1000;
			gpioPin_togglePin(pinPTB17);
		} else {
			waittime = 20;
		}
		vTaskDelayUntil(&lastWakeUpTime, waittime / portTICK_PERIOD_MS);
	}
}

#if CONFIG_USE_STATS_FORMATTING_FUNCTIONS > 0
void taskManTask(void *data) {
	TickType_t lastWakeUpTime = xTaskGetTickCount();
	static char taskBuff[5 * 1024];
	for(;;) {
		vTaskList(taskBuff);
		printf("name\t\tState\tPrio\tStack\tTaskNr.\n");
		printf("%s", taskBuff);
		printf("blocked ('B'), ready ('R'), deleted ('D') or suspended ('S')\n");
#if CONFIG_GENERATE_RUN_TIME_STATS > 0
		printf("name\t\tTime\t\t%%\n");
		vTaskGetRunTimeStats(taskBuff);
		printf("%s", taskBuff);
#endif
		printf("\n");
		vTaskDelayUntil(&lastWakeUpTime, 1000 / portTICK_PERIOD_MS);
	}
}
#endif

#ifdef CONFIG_MOTORTEST
void motor_testTask(void *data) {
	struct motor *motor = data;
	TickType_t lastWakeUpTime = xTaskGetTickCount();
	int n = 1000;
	bool up = true;
	for (;;) {
		motor_set(motor, MOTOR_PIN1, n);
		motor_set(motor, MOTOR_PIN2, n);
		motor_set(motor, MOTOR_PIN3, n);
		motor_set(motor, MOTOR_PIN4, n);
		if (up) {
			n += 10;
		} else {
			n -= 10;
		}
		if (n >= 1100) {
			up = false;
		}
		if (n <= 1000) {
			up = true;
		}
		vTaskDelayUntil(&lastWakeUpTime, 100 / portTICK_PERIOD_MS);
	}
}
#endif
struct motor *motor;

#ifdef CONFIG_RC_MOTORTEST
void rcTestTask(void *data) {
	struct rc * rc = data;
	TickType_t lastWakeUpTime = xTaskGetTickCount();
	for (;;) {
		int32_t pin[6];
		pin[0] = rc_get(rc, 0);
		pin[1] = rc_get(rc, 1);
		pin[2] = rc_get(rc, 2);
		pin[3] = rc_get(rc, 3);
		pin[4] = rc_get(rc, 4);
		pin[5] = rc_get(rc, 5);
		printf("pin 0: %04ld 1: %04ld 2: %04ld 3: %04ld 4: %04ld 5: %04ld\n", pin[0], pin[1], pin[2], pin[3], pin[4], pin[5]);
		motor_set(motor, MOTOR_PIN1, pin[5]);
		motor_set(motor, MOTOR_PIN2, pin[5]);
		motor_set(motor, MOTOR_PIN3, pin[5]);
		motor_set(motor, MOTOR_PIN4, pin[5]);

		
		vTaskDelayUntil(&lastWakeUpTime, 10 / portTICK_PERIOD_MS);
	}
}
#endif


void batTask(void *data) {
	struct adc *adc = adc_init(1, 12, 4125000);
	struct spi *spi;
	int32_t ret;
	struct tps65381 *tps ;
	struct spi_opt opt = {
		.lsb = false,
		.cpol = false,
		.cpha = true,
		.cs = 0,
		.csLowInactive = false,
		.gpio = SPI_OPT_GPIO_DIS,
		.size = 8, 
		.wdelay = 0,
		.cs_hold = 54,
		.cs_delay = 54,
		.bautrate = 500000
	};
	struct spi_slave *slave;
	float val;
	TickType_t lastWakeUpTime = xTaskGetTickCount();

	spi = spi_init(0, SPI_3WIRE_CS, NULL);
	CONFIG_ASSERT(spi != NULL);
	slave = spiSlave_init(spi, &opt);
	CONFIG_ASSERT(slave != NULL);
	tps = tps_init(slave, 100 / portTICK_PERIOD_MS);
	CONFIG_ASSERT(tps != NULL);
	for(;;) {
		ret = tps_mux(tps, TPS_VBAT, 100 / portTICK_PERIOD_MS);
		CONFIG_ASSERT(ret == 0);
		vTaskDelay(10 / portTICK_PERIOD_MS);
		val = tps_diag(tps, TPS_VBAT, adc, 100 / portTICK_PERIOD_MS);
		printf("TPS_VBAT : %f V\n", val);
		if (val <= 10) {
			printf("Voltag to low!!\n");
			taskDISABLE_INTERRUPTS(); /* Disable all Interrupts */
			vTaskSuspendAll(); /* Stop all Tasks */
			motor_set(motor, MOTOR_PIN1, 0);
			motor_set(motor, MOTOR_PIN2, 0);
			motor_set(motor, MOTOR_PIN3, 0);
			motor_set(motor, MOTOR_PIN4, 0);
			for(;;); /* Block in Intterrupt */
		}
		vTaskDelayUntil(&lastWakeUpTime, 250 / portTICK_PERIOD_MS);
	}
}

int main() {
	int32_t ret;
	ret = irq_init();
	CONFIG_ASSERT(ret == 0);
	struct timer *ftm;
	struct pwm *pwm;
	struct uart *uart = uart_init(1, 115200);
	struct lc *lc;
#ifdef CONFIG_NEWLIB
	ret = newlib_init(uart, uart);
	CONFIG_ASSERT(ret == 0);
#endif
	printf("Init Devices\n");
	ret = initGPIO();
	CONFIG_ASSERT(ret == 0);

	ftm = timer_init(1, 32, 20000, 700);
	CONFIG_ASSERT(ftm != NULL);
	pwm = pwm_init(EM_LED); 
	CONFIG_ASSERT(pwm != NULL);
	ret = pwm_setPeriod(pwm, 24000);
	CONFIG_ASSERT(ret == 0);
	ret = pwm_setDutyCycle(pwm, 10000);
	CONFIG_ASSERT(ret == 0);
	xTaskCreate(ledTask, "LED Task", 128, pwm, 1, NULL);
#if CONFIG_USE_STATS_FORMATTING_FUNCTIONS > 0
	xTaskCreate(taskManTask, "Task Manager Task", 512, NULL, 1, NULL);
#endif
	{
		struct timer *timer = timer_init(0, 32, 20000, 700);
		CONFIG_ASSERT(timer != NULL);
		motor = motor_init();
		motor_enable(motor, MOTOR_PIN1);
		motor_enable(motor, MOTOR_PIN2);
		motor_enable(motor, MOTOR_PIN3);
		motor_enable(motor, MOTOR_PIN4);
#ifdef CONFIG_MOTORTEST
		xTaskCreate(motor_testTask, "Motor Test Task", 512, motor, 1, NULL);
#endif
#ifdef CONFIG_MOTOR_OFF
		/* 
		 * Correct Signal but is not smaler as minimum ESC do nothing. Init Sequenz of ESC is:
		 * < 1070 then > 1070 for more speed
		 */
		motor_set(motor, MOTOR_PIN1, 1100); 
		motor_set(motor, MOTOR_PIN2, 1100);
		motor_set(motor, MOTOR_PIN3, 1100);
		motor_set(motor, MOTOR_PIN4, 1100);
#endif
	}
	{
		lc = lc_init(motor);
		CONFIG_ASSERT(lc != NULL);
	}
	{
		struct timer *timer = timer_init(1, 32, 20000, 700);
		CONFIG_ASSERT(timer != NULL);
		struct rc *rc = rc_init(timer);
		CONFIG_ASSERT(rc != NULL);
		{
			struct capture *cap = capture_init(RC_PIN1);
			CONFIG_ASSERT(cap != NULL);
			ret = rc_setup(rc, cap);
			CONFIG_ASSERT(ret >= 0);
		}
		{
			struct capture *cap = capture_init(RC_PIN2);
			CONFIG_ASSERT(cap != NULL);
			ret = rc_setup(rc, cap);
			CONFIG_ASSERT(ret >= 0);
		}
		{
			struct capture *cap = capture_init(RC_PIN3);
			CONFIG_ASSERT(cap != NULL);
			ret = rc_setup(rc, cap);
			CONFIG_ASSERT(ret >= 0);
		}
		{
			struct capture *cap = capture_init(RC_PIN4);
			CONFIG_ASSERT(cap != NULL);
			ret = rc_setup(rc, cap);
			CONFIG_ASSERT(ret >= 0);
		}
		{
			struct capture *cap = capture_init(RC_PIN5);
			CONFIG_ASSERT(cap != NULL);
			ret = rc_setup(rc, cap);
			CONFIG_ASSERT(ret >= 0);
		}
		{
			struct capture *cap = capture_init(RC_PIN6);
			CONFIG_ASSERT(cap != NULL);
			ret = rc_setup(rc, cap);
			CONFIG_ASSERT(ret >= 0);
		}
#ifdef CONFIG_RC_MOTORTEST
		xTaskCreate(rcTestTask, "RC Test task", 512, rc, 1, NULL);
#endif
	}
	xTaskCreate(batTask, "Bat task", 512, NULL, 4, NULL);
	printf("Start Scheduler\n");
	systemLaunch();
	vTaskStartScheduler ();
	for(;;);
	return 0;
}
