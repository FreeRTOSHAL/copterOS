/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include "commander.h"
#include <remote_control.h>

/* Stabilizer Variable */
#if 0
extern Axis3f gyro; // Gyro axis data in deg/s
extern Axis3f acc;  // Accelerometer axis data in mG
extern Axis3f mag;  // Magnetometer axis data in testla
#endif

extern float eulerRollActual;   // Measured roll angle in deg
extern float eulerPitchActual;  // Measured pitch angle in deg
extern float eulerYawActual;    // Measured yaw angle in deg
extern float eulerRollDesired;  // Desired roll angle in deg
extern float eulerPitchDesired; // Desired ptich angle in deg
extern float eulerYawDesired;   // Desired yaw angle in deg
extern float rollRateDesired;   // Desired roll rate in deg/s
extern float pitchRateDesired;  // Desired pitch rate in deg/s
extern float yawRateDesired;    // Desired yaw rate in deg/s

extern float carefreeFrontAngle; // carefree front angle that is set

extern uint16_t actuatorThrust;  // Actuator output for thrust base
extern int16_t  actuatorRoll;    // Actuator output roll compensation
extern int16_t  actuatorPitch;   // Actuator output pitch compensation
extern int16_t  actuatorYaw;     // Actuator output yaw compensation

extern uint32_t motorPowerM1;  // Motor 1 power output (16bit value used: 0 - 65535)
extern uint32_t motorPowerM2;  // Motor 2 power output (16bit value used: 0 - 65535)
extern uint32_t motorPowerM3;  // Motor 3 power output (16bit value used: 0 - 65535)
extern uint32_t motorPowerM4;  // Motor 4 power output (16bit value used: 0 - 65535)
#define PRINT_DISPLAY(fmt, ...) printf("DISPLAY: " fmt, ##__VA_ARGS__)
#define PRINT_VAL(type, name) printf("\t" #name ": \t" type, name)

static void displayTask(void *data) {
	(void) data;

	TickType_t lastWakeUpTime = xTaskGetTickCount();
	for(;;) {
		printf("\n");
		PRINT_DISPLAY("Actual:");
		PRINT_VAL("%f", eulerRollActual);
		PRINT_VAL("%f", eulerPitchActual);
		PRINT_VAL("%f", eulerYawActual);
		printf("\n");
		PRINT_DISPLAY("Desired:");
		PRINT_VAL("%f", eulerRollDesired);
		PRINT_VAL("%f", eulerPitchDesired);
		PRINT_VAL("%f", eulerYawDesired);
		printf("\n");
		PRINT_DISPLAY("Calculated:");
		PRINT_VAL("%f", rollRateDesired);
		PRINT_VAL("%f", pitchRateDesired);
		PRINT_VAL("%f", yawRateDesired);
		printf("\n");
		PRINT_DISPLAY("");
		PRINT_VAL("%f", carefreeFrontAngle);

		printf("\n");
		PRINT_DISPLAY("Actual Values");
		PRINT_VAL("%u", actuatorThrust);
		PRINT_VAL("%d", actuatorRoll);
		PRINT_VAL("%d", actuatorPitch);
		PRINT_VAL("%d", actuatorYaw);
		printf("\n");
		PRINT_DISPLAY("Actual Motor Values");
		PRINT_VAL("%lu", motorPowerM1);
		PRINT_VAL("%lu", motorPowerM2);
		PRINT_VAL("%lu", motorPowerM3);
		PRINT_VAL("%lu", motorPowerM4);
		printf("\n");
#if 0
		{
			float roll = (float) rc_get(comm.rc, comm.rollID);
			float pitch = (float) rc_get(comm.rc, comm.pitchID);
			float yaw = (float) rc_get(comm.rc, comm.yawID);
			float thrust = (float) rc_get(comm.rc, comm.thrustID);
			PRINT_DISPLAY("Actual RC Values");
			PRINT_VAL("%f", roll);
			PRINT_VAL("%f", pitch);
			PRINT_VAL("%f", yaw);
			PRINT_VAL("%f", thrust);
		}
#endif
		printf("\n");
		{
			float commanderR;
			float commanderP;
			float commanderY;
			uint16_t commanderT;
			commanderGetRPY(&commanderR, &commanderP, &commanderY);
			commanderGetThrust(&commanderT);
			PRINT_DISPLAY("Actual Calc RC Values");
			PRINT_VAL("%f", commanderR);
			PRINT_VAL("%f", commanderP);
			PRINT_VAL("%f", commanderY);
			PRINT_VAL("%u", commanderT);
		}
		printf("\n");

		vTaskDelayUntil(&lastWakeUpTime, 250 / portTICK_PERIOD_MS);
	}

}
void display_init() {
	xTaskCreate(displayTask, "Dispaly Task", 500, NULL, 1, NULL);
}
