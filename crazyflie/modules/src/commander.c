#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include <remote_control.h>
#include <linux_client.h>

#include "commander.h"

struct commander {
	bool isInit;
	bool isInactive;
	bool thrustLocked;
	void (*getThrust)(uint16_t* thrust);
	void (*getRPY)(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired);
	RPYType stabilizationModeRoll;; // Current stabilization type of roll (rate or angle)
	RPYType stabilizationModePitch; // Current stabilization type of pitch (rate or angle)
	RPYType stabilizationModeYaw;// Current stabilization type of yaw (rate or angle)
	YawModeType yawMode; // Yaw mode configuration
};

static struct commander comm = {
	.isInit = false,
	.stabilizationModeRoll = ANGLE,
	.stabilizationModePitch  = ANGLE,
	.stabilizationModeYaw = RATE,
	.yawMode = DEFUALT_YAW_MODE,
	.getThrust = NULL,
	.getRPY = NULL,
};

void commanderInit(void) {
	if(comm.isInit)
		return;
	
	comm.thrustLocked = false;
	comm.isInit = true;
}
bool commanderTest(void) {
	return comm.isInit;
}
void commanderWatchdog(void) {
}
uint32_t commanderGetInactivityTime(void) {
	return  0; /* TODO: ?? */
}

void commanderGetRPY(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired) {
	if (comm.getRPY) {
		comm.getRPY(eulerRollDesired, eulerPitchDesired, eulerYawDesired);
	} else {
		*eulerRollDesired = 0;
		*eulerPitchDesired = 0;
		*eulerYawDesired = 0;
	}
}
void commanderGetRPYType(RPYType* rollType, RPYType* pitchType, RPYType* yawType) {
	*rollType  = comm.stabilizationModeRoll;
	*pitchType = comm.stabilizationModePitch;
	*yawType   = comm.stabilizationModeYaw;
}
void commanderSetRPYType(RPYType rollType, RPYType pitchType, RPYType yawType) {
	comm.stabilizationModeRoll = rollType;
	comm.stabilizationModePitch = pitchType;
	comm.stabilizationModeYaw = yawType;
}
void commanderGetThrust(uint16_t* thrust) {
	if (comm.getThrust) {
		comm.getThrust(thrust);
	} else {
		*thrust = 0;
	}
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
	return comm.yawMode;
}
bool commanderGetYawModeCarefreeResetFront(void) {
	return false;
}

void commanderLockThrust(void (*callback)(uint16_t* thrust), bool lock) {
	comm.thrustLocked = lock; /* Fist lock and then set */
	comm.getThrust = callback;
}

void commanderSetThrust(void (*callback)(uint16_t* thrust)) {
	if (!comm.thrustLocked) {
		comm.getThrust = callback;
	}
}
void commanderSetRPY(void (*callback)(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired)) {
	comm.getRPY = callback;
}
