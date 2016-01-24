#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include <spi.h>
#include <adc.h>
#include <tps65381.h>
#include <motor_ctrl.h>
#include <commander.h>

#define PRINT_EMER(fmt, ...) printf("emergency: " fmt, ##__VA_ARGS__)
#define PRINT_BAT(fmt, ...) printf("BAT: " fmt, ##__VA_ARGS__)

/* TODO Move to CONFIG */
#define MOTOR_PIN1 3
#define MOTOR_PIN2 6
#define MOTOR_PIN3 0
#define MOTOR_PIN4 2

struct emergency {
	bool shutdown;
	uint16_t thrust;
	TaskHandle_t emergency;
	struct motor *motor;
};

struct emergency emer = {
	.shutdown = false,
	.thrust = 0,
};

static void emergencyTask(void *data);
static void batTask(void *data);
struct emergency *emergency_init(struct motor *motor) {
	emer.motor = motor;
	xTaskCreate(emergencyTask, "Emergency Task", 512, &emer, CONFIG_MAX_PRIORITIES, &emer.emergency);
	xTaskCreate(batTask, "Bat task", 512, &emer, CONFIG_MAX_PRIORITIES, &emer.emergency);
	return &emer;
}
int32_t emergency_deinit(struct emergency *emer) {
	return 0;
}

void emergency_landing() {
	vTaskResume(emer.emergency);
}

bool emergency_landingISR() {
	return xTaskResumeFromISR(emer.emergency);
}

void emergency_shutdown() {
	PRINT_EMER("emergency shutdown forced\n");
	PRINT_EMER("Disable all Interrupts and supsend all Task\n");
	taskDISABLE_INTERRUPTS(); /* Disable all Interrupts */
	vTaskSuspendAll(); /* Stop all Tasks */
	motor_set(emer.motor, MOTOR_PIN1, 0);
	motor_set(emer.motor, MOTOR_PIN2, 0);
	motor_set(emer.motor, MOTOR_PIN3, 0);
	motor_set(emer.motor, MOTOR_PIN4, 0);
}

void batThrustContoll(uint16_t* t) {
	*t = emer.thrust;
}

static void emergencyTask(void *data) {
	for(;;) {
		vTaskSuspend(NULL);
#ifdef CONFIG_CRAZYFLIE
		if (!emer.shutdown) {
			emer.shutdown = true;
			/* 
			 * Check Thrust > 0
			 * if > 0 go to 40 % and then to 0
			 * else fixed at 0
			 */
			commanderGetThrust(&emer.thrust);
			if (emer.thrust > 0) {
				emer.thrust = (uint16_t) (((float) UINT16_MAX) * 0.4);
			}
			PRINT_EMER("Overwirde Thurst Contol\n");
			commanderLockThrust(batThrustContoll, true);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			PRINT_EMER("shutdown and save Copter\n");
			emer.thrust = 0;
		}
#endif
	}
}

static void batTask(void *data) {
	struct tps65381 *tps;
	struct adc *adc;
	struct spi *spi;
	int32_t ret;
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
	adc = adc_init(1, 12, 4125000);
	spi = spi_init(0, SPI_3WIRE_CS, NULL);
	CONFIG_ASSERT(spi != NULL);
	slave = spiSlave_init(spi, &opt);
	CONFIG_ASSERT(slave != NULL);
	tps = tps_init(slave, 100 / portTICK_PERIOD_MS);
	CONFIG_ASSERT(tps != NULL);
	ret = tps_mux(tps, TPS_VBAT, 100 / portTICK_PERIOD_MS);
	CONFIG_ASSERT(ret == 0);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	for(;;) {
		val = tps_diag(tps, TPS_VBAT, adc, 100 / portTICK_PERIOD_MS);
		PRINT_BAT("TPS_VBAT : %f V\n", val);
		if (val <= 10) { /* TODO move to CONFIG */
			PRINT_EMER("Voltag to low!!\n");
			emergency_landing();
		}
		if (val <= 9.5) { /* TODO move to CONFIG */
			PRINT_EMER("Baterie drop detected!\n");
			emergency_shutdown();
		}
		vTaskDelayUntil(&lastWakeUpTime, 250 / portTICK_PERIOD_MS);
	}
}
