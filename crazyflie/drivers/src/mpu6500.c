#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include <mpu9250.h>
#include "mpu6500.h"
#include "config.h"
#define DEBUG_MODULE "IMU"
#include "debug.h"
struct mpu9250 *mpu;
struct accel *accel;
struct gyro *gyro;
/* 
 * Dummy implemention for MPU9250 for FreeRTOS HAL 
 */
void mpu6500Init() {
	{
		/* TODO not do it hear*/
		struct spi *spi;
		struct spi_slave *slave[3];
		{
			struct spi_opt opt = {
				.lsb = false,
				.cpol = false,
				.cpha = false,
				.cs = 0,
				.csLowInactive = false,
				.gpio = SPI_OPT_GPIO_DIS,
				.size = 8,
				.wdelay = 0,
				.cs_hold = 8,
				.cs_delay = 500,
				.bautrate = 500000,
			};
			spi = spi_init(1, SPI_3WIRE_CS, NULL);
			CONFIG_ASSERT(spi != NULL);
			/*slave[0] = spiSlave_init(spi, &opt);
			CONFIG_ASSERT(slave[0] != NULL);*/ /* Init by Driver*/
			opt.cs_hold = 6;
			opt.cs_delay = 8;
			opt.cs = 1;
			slave[1] = spiSlave_init(spi, &opt);
			CONFIG_ASSERT(slave[1] != NULL);
			opt.cs = 2;
			slave[2] = spiSlave_init(spi, &opt);
			CONFIG_ASSERT(slave[2] != NULL);
		}
		mpu = mpu9250_init(0, portMAX_DELAY);
		CONFIG_ASSERT(mpu != NULL);
		accel = accel_init(0);
		CONFIG_ASSERT(accel != NULL);
		gyro = gyro_init(0);
		CONFIG_ASSERT(gyro != NULL);
	}
}
bool mpu6500SelfTest() {
	return mpu != NULL && accel != NULL && gyro != NULL;
}
void mpu6500SetIntEnabled(uint8_t enabled) {
	return;
}
bool mpu6500TestConnection() {
	return mpu != NULL && accel != NULL && gyro != NULL;
}
void mpu6500Reset() {
	return;
}
void mpu6500SetSleepEnabled(bool enabled) {
	return;
}
bool mpu6500GetSleepEnabled() {
	return false;
}
bool mpu6500GetTempSensorEnabled() {
	return true;
}
void mpu6500SetTempSensorEnabled(bool enabled) {
	return;
}
bool mpu6500GetI2CBypassEnabled() {
	return true;
}
void mpu6500SetI2CBypassEnabled(bool enabled) {
	return;
}
void mpu6500SetClockSource(uint8_t source) {
	return;
}
void mpu6500SetFullScaleGyroRange(uint8_t range) {
	return;
}
void mpu6500SetFullScaleAccelRange(uint8_t range) {
	return;
}
void mpu6500SetDLPFMode(uint8_t bandwidth) {
	return;
}
void mpu6500SetRate(uint8_t rate) {
	return;
}
void mpu6500GetMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
	struct vector a;	
	struct vector g;
	int32_t ret;
	ret = accel_get(accel, &a, M2T(100));
	CONFIG_ASSERT(ret >= 0);
	ret = gyro_get(gyro, &g, M2T(100));
	CONFIG_ASSERT(ret >= 0);
	*ax = -a.x;
	*ay = -a.y;
	*az = a.z;

	*gx = -g.x;
	*gy = -g.y;
	*gz = g.z;
	/*DEBUG_PRINT("ax: %d ay: %d az: %d gx: %d gy: %d gz: %d\n", -a.x, -a.y, a.z, -g.x, -g.y, g.z);*/
}
