#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include <remote_control.h>
#include <linux_client.h>

#include "commander.h"

#ifdef CONFIG_DAVID_RC
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
#else
/* Uli RC Base Values */
#define ROLL_BASE 1494.
#define ROLL_MIN 1104.
#define ROLL_MAX 1895.

#define PITCH_BASE 1497.
#define PITCH_MIN 1094.
#define PITCH_MAX 1895.

#define YAW_BASE 1485.
#define YAW_MIN 1094.
#define YAW_MAX 1895.

#define THRUST_BASE 1137.
#define THRUST_MIN 1097.
#define THRUST_MAX 1895.
#endif
#define EULER_ROLL_MAX 15.
#define EULER_PITCH_MAX 15.
#define EULER_YAW_MAX 130.

#define MIN_RP 0.03
#define MIN_Y 0.05

struct commander comm = {
	.isInit = false,
	.rollID = 3,
	.pitchID = 1,
	.yawID = 0,
	.thrustID = 5,
};

static RPYType stabilizationModeRoll  = ANGLE; // Current stabilization type of roll (rate or angle)
static RPYType stabilizationModePitch = ANGLE; // Current stabilization type of pitch (rate or angle)
static RPYType stabilizationModeYaw   = RATE;  // Current stabilization type of yaw (rate or angle)

static YawModeType yawMode = DEFUALT_YAW_MODE; // Yaw mode configuration



static void spectrumGetThrust(uint16_t* thrust);
static void spectrumGetRPY(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired);
void (*getThrust)(uint16_t* thrust) = &spectrumGetThrust;
void (*getRPY)(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired) = &spectrumGetRPY;

static void lc_select(struct lc *lc, struct lc_msg *msg);
static void lc_control(struct lc *lc, struct lc_msg *msg);
void commanderInit(void) {
	struct lc *lc = lc_init();
	if(comm.isInit)
		return;
	
	comm.thrustLocked = true;
	comm.isInit = true;
	comm.rc = rc_init(NULL); /* TODO: Init is in main */
	
	lc_registerCallback(lc, LC_TYPE_SELECT, lc_select);
	lc_registerCallback(lc, LC_TYPE_CONTROL, lc_control);
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
	getRPY(eulerRollDesired, eulerPitchDesired,eulerYawDesired);
}
static void spectrumGetRPY(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired) {
#if 1
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

	pitch *= -1;
	roll *= -1;
	yaw *= -1;

	if (roll < -MIN_RP || roll > MIN_RP) {
		*eulerRollDesired = (roll * EULER_ROLL_MAX);
	} else {
		*eulerRollDesired = 0;
	}
	if (pitch < -MIN_RP || pitch > MIN_RP) {
		*eulerPitchDesired = (pitch * EULER_PITCH_MAX);
	} else {
		*eulerPitchDesired = 0;
	}
	if (yaw < -MIN_Y || yaw > MIN_Y) {
		*eulerYawDesired = (yaw * EULER_YAW_MAX);
	} else {
		*eulerYawDesired = 0;
	}
#else
	*eulerRollDesired = 0;
	*eulerPitchDesired = 0;
	*eulerYawDesired = 0;
#endif

	//printf("roll: %f pitch: %f yaw: %f\n", *eulerRollDesired, *eulerPitchDesired, *eulerYawDesired);
}
void commanderGetRPYType(RPYType* rollType, RPYType* pitchType, RPYType* yawType) {
	*rollType  = stabilizationModeRoll;
	*pitchType = stabilizationModePitch;
	*yawType   = stabilizationModeYaw;
}
void commanderSetRPYType(RPYType rollType, RPYType pitchType, RPYType yawType) {
	stabilizationModeRoll = rollType;
	stabilizationModePitch = pitchType;
	stabilizationModeYaw = yawType;
}
void commanderGetThrust(uint16_t* thrust) {
	getThrust(thrust);
}
static void spectrumGetThrust(uint16_t* thrust) {
	float y = (float) rc_get(comm.rc, comm.yawID);
	float t = (float) rc_get(comm.rc, comm.thrustID);
	if (t == 0 || y == 0) {
		comm.thrustLocked = true;
		*thrust = 0;
		return;
	}
	//printf("t: %f\n", t);
	t = getPercent(t, THRUST_BASE, THRUST_MIN, THRUST_MAX);
	if (t < 0) {
		comm.thrustLocked = false;
		*thrust = 0;
		return;
	}
	if (comm.thrustLocked) {
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

void commanderSelectSpectrum(void) {
	getThrust = &spectrumGetThrust;
	getRPY = &spectrumGetRPY;
}
/**
 * Autocopt Control Message
 */
struct autocopt_control {
	/**
	 * -1 left +1 right
	 */ 
	float roll;
	/**
	 * -1 back +1 forward
	 */
	float pitch;
	/**
	 * -1 left +1 right
	 */
	float yaw;
	/**
	 * 0 = no Thrust
	 * 2^16 = full Thrust
	 * \waring Value below 60000 the copter may instable 
	 */
	uint16_t thrust;
} __attribute__((packed));

struct autocopt_control ctl;
static void lc_control(struct lc *lc, struct lc_msg *msg) {
	memcpy(&ctl, msg->data, sizeof(struct autocopt_control));
	lc_sendAct(lc);
}
static void linuxGetRPY(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired) {
#if 1
	float roll;
	float pitch;
	float yaw;
	roll = ctl.roll;
	pitch = -ctl.pitch;
	yaw = ctl.yaw;

	*eulerRollDesired = (roll * EULER_ROLL_MAX);
	*eulerPitchDesired = (pitch * EULER_PITCH_MAX);
	*eulerYawDesired = (yaw * EULER_YAW_MAX);
#else
	*eulerRollDesired = 0;
	*eulerPitchDesired = 0;
	*eulerYawDesired = 0;
#endif

	//printf("roll: %f pitch: %f yaw: %f\n", *eulerRollDesired, *eulerPitchDesired, *eulerYawDesired);
}
static void linuxGetThrust(uint16_t* thrust) {
	if (comm.thrustLocked && ctl.thrust == 0) {
		comm.thrustLocked = false;
	}
	if (comm.thrustLocked) {
		*thrust = 0;
		return;
	}
	*thrust = ctl.thrust;
}
void commanderSelectLinux(void) {
	getThrust = &linuxGetThrust;
	getRPY = &linuxGetRPY;
}
static void lc_select(struct lc *lc, struct lc_msg *msg) {
#ifdef CONFIG_SELECT_THRUST_LOCKED
	comm.thrustLocked = true;
#endif
	switch (msg->data[0]) {
		case 0: /* Move to DEFINE */
			commanderSelectSpectrum();
			break;
		case 1:
			commanderSelectLinux();
			break;
		default:
			lc_SendFailt(lc);
			return;
	}
	lc_sendAct(lc);
}
