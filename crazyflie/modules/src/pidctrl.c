/**
 *    ||          ____  _ __                           
 * +------+      / __ )(_) /_______________ _____  ___ 
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * pidctrl.c - Used to receive/answer requests from client and to receive updated PID values from client
 */
 
#include <stdio.h>
/* FreeRtos includes */
#include "FreeRTOS.h"
#include "task.h"

#include "crtp.h"
#include "pidctrl.h"
#include "pid.h"
#include "commander.h"
#include <linux_client.h>
void pidCrtlCallback(struct lc *lc, struct lc_msg *msg);

void pidCtrlInit() {
	struct lc *lc = lc_init();
	lc_registerCallback(lc, LC_TYPE_PID, &pidCrtlCallback);
}
extern PidObject pidRollRate;
extern PidObject pidPitchRate;
extern PidObject pidYawRate;
extern PidObject pidRoll;
extern PidObject pidPitch;
extern PidObject pidYaw;

#define PID_ATT_MODE_R 0
#define PID_ATT_MODE_P 1
#define PID_ATT_MODE_Y 2
#define IS_BIT_SET(x, p) (((x) >> p) & 0x1)
#define PID_RATE 0
#define PID_ATT 1
struct pidValuesHeader {
	uint8_t type;
	uint8_t mode;
} __attribute__((packed));
struct pidValues {
	struct pidValuesHeader header;
	uint16_t KpR;
	uint16_t KiR;
	uint16_t KdR;
	uint16_t KpP;
	uint16_t KiP;
	uint16_t KdP;
	uint16_t KpY;
	uint16_t KiY;
	uint16_t KdY;
} __attribute__((packed));


#define DEBUG_PID(ftm, ...) printf("PIDCTRL: " ftm, ##__VA_ARGS__)
#define SETKP(p, v) do { DEBUG_PID(#p " " #v "=%f\n", (v)); pidSetKp((p), (v));} while(0)
#define SETKI(p, v) do { DEBUG_PID(#p " " #v "=%f\n", (v)); pidSetKi((p), (v));} while(0)
#define SETKD(p, v) do { DEBUG_PID(#p " " #v "=%f\n", (v)); pidSetKd((p), (v));} while(0)

void pidCrtlCallback(struct lc *lc, struct lc_msg *msg)
{
	struct pidValues *pPid = (struct pidValues *)msg->data;
	switch (pPid->header.type) {
		case PID_RATE:
			DEBUG_PID("Set Rate:\n");
			SETKP(&pidRollRate, (float)pPid->KpR/100.0);
			SETKI(&pidRollRate, (float)pPid->KiR/100.0);
			SETKD(&pidRollRate, (float)pPid->KdR/100.0);
			SETKP(&pidPitchRate, (float)pPid->KpP/100.0);
			SETKI(&pidPitchRate, (float)pPid->KiP/100.0);
			SETKD(&pidPitchRate, (float)pPid->KdP/100.0);
			SETKP(&pidYawRate, (float)pPid->KpY/100.0);
			SETKI(&pidYawRate, (float)pPid->KiY/100.0);
			SETKD(&pidYawRate, (float)pPid->KdY/100.0);
			break;
		case PID_ATT:
			DEBUG_PID("Set ATT:\n");
			SETKP(&pidRoll, (float)pPid->KpR/100.0);
			SETKI(&pidRoll, (float)pPid->KiR/100.0);
			SETKD(&pidRoll, (float)pPid->KdR/100.0);
			SETKP(&pidPitch, (float)pPid->KpP/100.0);
			SETKI(&pidPitch, (float)pPid->KiP/100.0);
			SETKD(&pidPitch, (float)pPid->KdP/100.0);
			SETKP(&pidYaw, (float)pPid->KpY/100.0);
			SETKI(&pidYaw, (float)pPid->KiY/100.0);
			SETKD(&pidYaw, (float)pPid->KdY/100.0);
			break;
		default:
			DEBUG_PID("Error unkown type: %d", pPid->header.type);
			break;
	}

	commanderSetRPYType(
		(RPYType) IS_BIT_SET(pPid->header.mode, PID_ATT_MODE_R), 
		(RPYType) IS_BIT_SET(pPid->header.mode, PID_ATT_MODE_P), 
		(RPYType) IS_BIT_SET(pPid->header.mode, PID_ATT_MODE_Y)
	);
	lc_sendAct(lc);
}

