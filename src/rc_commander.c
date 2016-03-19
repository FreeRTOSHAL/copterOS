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
#include <commander.h>
#include <remote_control.h>
#include <rc_commander.h>
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
#define MIN_RP 0.03
#define MIN_Y 0.05

struct rcComm {
	bool init;
	struct rc *rc;
	bool thrustLocked;
	uint32_t rollID;
	uint32_t pitchID;
	uint32_t yawID;
	uint32_t thrustID;
};

struct rcComm rcComm = {
	.thrustLocked = true,
	/* IDs Pos in Array of remote_contol struct*/
	.rollID = 0,
	.pitchID = 1,
	.yawID = 2,
	.thrustID = 3,
};

struct rcComm *rcComm_init(struct rc *rc) {
	if (rcComm.init) {
		return &rcComm;
	}
	rcComm.init = true;
	rcComm.rc = rc;
#ifdef CONFIG_DEFAULT_RC_SPECTRUM
	rcComm_selectThrust();
	rcComm_selectRTY();
#endif
	return &rcComm;
}

int32_t rcComm_deinit() {
	rcComm.thrustLocked = true;
	return 0;
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
static void rcComm_GetThrust(uint16_t* thrust) {
	float y = (float) rc_get(rcComm.rc, rcComm.yawID);
	float t = (float) rc_get(rcComm.rc, rcComm.thrustID);
	if (t == 0 || y == 0) {
		rcComm.thrustLocked = true;
		*thrust = 0;
		return;
	}
	//printf("t: %f\n", t);
	t = getPercent(t, THRUST_BASE, THRUST_MIN, THRUST_MAX);
	if (t < 0) {
		rcComm.thrustLocked = false;
		*thrust = 0;
		return;
	}
	if (rcComm.thrustLocked) {
		*thrust = 0;
		return;
	}
	*thrust = (uint16_t) (t * UINT16_MAX);
	//printf("thrust: %u\n", *thrust);
}
static void rcComm_GetRPY(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired) {
#if 1
	float roll = (float) rc_get(rcComm.rc, rcComm.rollID);
	float pitch = (float) rc_get(rcComm.rc, rcComm.pitchID);
	float yaw = (float) rc_get(rcComm.rc, rcComm.yawID);
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
}
void rcComm_selectThrust() {
	commanderSetThrust(&rcComm_GetThrust);
}
void rcComm_selectRTY() {
	commanderSetRPY(&rcComm_GetRPY);
}
