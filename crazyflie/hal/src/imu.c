#include <FreeRTOS.h>
#include <task.h>
#include <mpu9250.h>
#include <accel.h>
#include <gyro.h>
#include <imu.h>
#include <imu_types.h>


struct mpu9250 *mpu;
struct accel *accel;
struct gyro *gyro;

void imu6Init(void) {
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
bool imu6Test(void) {
	/* Done by init */
	return mpu != NULL && accel != NULL && gyro != NULL;
}
bool imu6ManufacturingTest(void) {
	/* Done by init */
	return mpu != NULL && accel != NULL && gyro != NULL;
}
void imu6Read(Axis3f* gyro, Axis3f* acc) {
	struct mpu9250_vector g;
	struct mpu9250_vector a;
	int32_t ret;
	ret = mpu9250_getAccel(mpu, &a, portMAX_DELAY);
	CONFIG_ASSERT(ret >= 0);
	ret = mpu9250_getGyro(mpu, &g, portMAX_DELAY);
	CONFIG_ASSERT(ret >= 0);
	gyro->x = g.x;
	gyro->y = g.y;
	gyro->z = g.z;

	acc->x = a.x;
	acc->y = a.y;
	acc->z = a.z;
}
void imu9Read(Axis3f* gyroOut, Axis3f* accOut, Axis3f* magOut) {
	struct mpu9250_vector g;
	struct mpu9250_vector a;
	int32_t ret;
	ret = mpu9250_getAccel(mpu, &a, portMAX_DELAY);
	CONFIG_ASSERT(ret >= 0);
	ret = mpu9250_getGyro(mpu, &g, portMAX_DELAY);
	CONFIG_ASSERT(ret >= 0);
	/* Convert in crazyflie condinationn system */
	gyroOut->x = g.x;
	gyroOut->y = g.y;
	gyroOut->z = g.z;

	accOut->x = a.x;
	accOut->y = a.y;
	accOut->z = a.z;

	magOut->x = 0;
	magOut->y = 0;
	magOut->z = 0;
}
bool imu6IsCalibrated(void) {
	return mpu != NULL && accel != NULL && gyro != NULL;
}
bool imuHasBarometer(void) {
	return false;
}
bool imuHasMangnetometer(void) {
	return false; /* TODO MPU has Magnetometer */
}
