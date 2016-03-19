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
#include <string.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "config.h"
#include "pm.h"
void pmInit(void);

bool pmTest(void);

/**
 * Power management task
 */
void pmTask(void *param) {

}

void pmSetChargeState(PMChargeStates chgState) {

}
void pmSyslinkUpdate(SyslinkPacket *slp) {

}

/**
 * Returns the battery voltage i volts as a float
 */
float pmGetBatteryVoltage(void) {
	return 3.3; /* TODO */
}

/**
 * Returns the min battery voltage i volts as a float
 */
float pmGetBatteryVoltageMin(void) {
	return 3.0; /* TODO */
}

/**
 * Returns the max battery voltage i volts as a float
 */
float pmGetBatteryVoltageMax(void) {
	return 4.2; /* TODO */
}

/**
 * Updates and calculates battery values.
 * Should be called for every new adcValues sample.
 */
void pmBatteryUpdate(AdcGroup* adcValues) {
	
}

/**
 * Returns true if the battery is currently in use
 */
bool pmIsDischarging(void) {
	return true;
}

