#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include <remote_control.h>

#include "commander.h"

struct commander {
	bool isInit;
	bool isInactive;
	bool thrustLocked;
	struct rc *rc;
	uint32_t rollID;
	uint32_t pitchID;
	uint32_t yawID;
	uint32_t thrustID;
};
#define ROLL_BASE 1494.
#define ROLL_MIN 1093.
#define ROLL_MAX 1894.
#define PITCH_BASE 1494.
#define PITCH_MIN 1093.
#define PITCH_MAX 1896.
#define YAW_BASE 1494.
#define YAW_MIN 1135.
#define YAW_MAX 1836.
#define THRUST_BASE 1060.
#define THRUST_MIN 999.
#define THRUST_MAX 1895.
#define EULER_ROLL_MAX 10.
#define EULER_PITCH_MAX 10.
#define EULER_YAW_MAX 10.

static struct commander comm = {
	.isInit = false,
	.rollID = 1,
	.pitchID = 3,
	.yawID = 0,
	.thrustID = 5,
};

static RPYType stabilizationModeRoll  = ANGLE; // Current stabilization type of roll (rate or angle)
static RPYType stabilizationModePitch = ANGLE; // Current stabilization type of pitch (rate or angle)
static RPYType stabilizationModeYaw   = RATE;  // Current stabilization type of yaw (rate or angle)

static YawModeType yawMode = DEFUALT_YAW_MODE; // Yaw mode configuration

void commanderInit(void) {
	if(comm.isInit)
		return;
	
	comm.isInactive = true;
	comm.thrustLocked = true;
	comm.isInit = true;
	comm.rc = rc_init(NULL); /* TODO: Init is in main */
}
bool commanderTest(void) {
	return comm.isInit && comm.rc != NULL;
}
void commanderWatchdog(void) {
}
uint32_t commanderGetInactivityTime(void) {
	return  0; /* TODO: ?? */
}

static inline float getPercent(float value, const float base, const float min, const float max) {
	value-=base;
	if (value > 0) {
		value /= (max - base);
	} else {
		value /= (min - base);
		value *= -1;
	}
	return value;
}

void commanderGetRPY(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired) {
#if 0
	float roll = (float) rc_get(comm.rc, comm.rollID);
	float pitch = (float) rc_get(comm.rc, comm.pitchID);
	float yaw = (float) rc_get(comm.rc, comm.yawID);
	//printf("roll: %f pitch: %f yaw: %f\n", roll, pitch, yaw);
	/* check rc connection */
	if (roll == 0 || pitch == 0 || yaw == 0) {
		*eulerRollDesired = 0;
		*eulerPitchDesired = 0;
		*eulerYawDesired = 0;
		return;
	}
	roll = getPercent(roll, ROLL_BASE, ROLL_MIN, ROLL_MAX);
	pitch = getPercent(pitch, ROLL_BASE, ROLL_MIN, ROLL_MAX);
	yaw = getPercent(yaw, ROLL_BASE, ROLL_MIN, ROLL_MAX);

	*eulerRollDesired = (roll * EULER_ROLL_MAX);
	*eulerPitchDesired = (pitch * EULER_PITCH_MAX);
	*eulerYawDesired = (yaw * EULER_YAW_MAX);
#endif
	*eulerRollDesired = 0;
	*eulerPitchDesired = 0;
	*eulerYawDesired = 0;

	//printf("roll: %f pitch: %f yaw: %f\n", *eulerRollDesired, *eulerPitchDesired, *eulerYawDesired);
}
void commanderGetRPYType(RPYType* rollType, RPYType* pitchType, RPYType* yawType) {
	*rollType  = stabilizationModeRoll;
	*pitchType = stabilizationModePitch;
	*yawType   = stabilizationModeYaw;
}
void commanderGetThrust(uint16_t* thrust) {
	float t = (float) rc_get(comm.rc, comm.thrustID);
	if (t == 0) {
		*thrust = 0;
		return;
	}
	//printf("t: %f\n", t);
	t = getPercent(t, THRUST_BASE, THRUST_MIN, THRUST_MAX);
	if (t < 0) {
		*thrust = 0;
		return;
	}
	*thrust = (uint16_t) (t * UINT16_MAX);
	//printf("thrust: %u\n", *thrust);
}
void commanderGetAltHold(bool* altHold, bool* setAltHold, float* altHoldChange) {
}
bool commanderGetAltHoldMode(void) {
	return false;
}
void commanderSetAltHoldMode(bool altHoldModeNew) {
	(void) altHoldModeNew;
}
YawModeType commanderGetYawMode(void) {
	return yawMode;
}
bool commanderGetYawModeCarefreeResetFront(void) {
	return false;
}