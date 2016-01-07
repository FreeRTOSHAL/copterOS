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

