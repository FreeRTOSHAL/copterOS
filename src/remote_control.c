#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <remote_control.h>
#include <timer.h>
#include <capture.h>
struct rc;
struct rc_channel {
	struct rc *rc;
	struct capture *capture;
	uint32_t raw;
	uint32_t value;
	bool retry;
	bool corret;
};
struct rc {
	struct rc_channel channel[RC_MAX_CHANNELS];
	struct timer *timer;
};


static bool rc_IRQHandler(struct capture *capture, uint32_t c, uint64_t time, void *data) {
	struct rc_channel *channel = data;
	uint32_t tmp_1 = channel->raw;
	uint32_t tmp_0 = (uint32_t) time;
	uint32_t tmp;

	if (tmp_0 > tmp_1) {
		tmp = tmp_0 - tmp_1;
	} else {
		/* 
		 * Overflow
		 */
		tmp = (UINT16_MAX - tmp_1) + tmp_0;
	}
	/* 
	 * Detect is Short or Long Impulse 
	 * PPM Motor / Servo Signal is approx 20ms long
	 * The Impulses is at the beginning of the signal
	 * The signal is min 0.5 ms and max 2 ms
	 * 
	 * The Interrupt is generated at begging of 
	 * rising edge and at falling edge 
	 * 
	 *1 |  ___         ___
	 *  |  |  |        |  |
	 *0 |__|  |___...__|  |
	 *  +--|--|--|--|--|--|
	 *  0  1  2  3 ... 20ms
	 *      ^ Impulse    ^ Impulse   
	 */
	if (tmp > RC_MIN && tmp < RC_MAX) {
		channel->value = tmp;
		channel->corret = true;
		channel->retry = true;
	}

	/* Save old Value for next Calculation */
	channel->raw = tmp_0;
	return 0;
}

static bool rc_OverfowHandler(struct timer *ftm, void *data) {
	struct rc *rc = data;
	int i;
	for (i = 0; i < RC_MAX_CHANNELS; i++) {
		/* 
		 * Wait unil next Overfow if !retry = value is not corect
		 */
		if (rc->channel[i].retry) {
			rc->channel[i].retry = false;
		} else {
			rc->channel[i].corret = false;
		}
	}
	return 0;
}


struct rc *rc_init(struct timer *timer) {
	int32_t ret;
	struct rc *rc = pvPortMalloc(sizeof(struct rc));
	if (rc == NULL) {
		goto rc_init_error_0;
	}
	memset(rc, 0, sizeof(struct rc));
	rc->timer = timer;
	ret = timer_setOverflowCallback(rc->timer, rc_OverfowHandler, rc);
	if (ret < 0) {
		goto rc_init_error_1;
	}
	return rc;
rc_init_error_1:
	vPortFree(rc);
rc_init_error_0:
	return NULL;
}

int32_t rc_setup(struct rc *rc, struct capture *capture) {
	int32_t ret;
	int index = 0;
	for (index = 0; index < RC_MAX_CHANNELS; index++) {
		if (rc->channel[index].capture == NULL) {
			break;
		}
	}
	if (index == RC_MAX_CHANNELS) {
		goto rc_setup_error_0;
	}
	rc->channel[index].raw = 0;
	rc->channel[index].value = 0;
	rc->channel[index].retry = 0;
	rc->channel[index].corret = 0;
	rc->channel[index].rc = rc;
	rc->channel[index].capture = capture;
	ret = capture_setCallback(rc->channel[index].capture, &rc_IRQHandler, &rc->channel[index]);
	if (ret < 0) {
		goto rc_setup_error_1;
	}
	ret = capture_setPeriod(capture, 24000);
	if (ret < 0) {
		goto rc_setup_error_0;
	}
	return index;
rc_setup_error_1:
	rc->channel[index].capture = NULL;
rc_setup_error_0:
	return ret;
}
uint32_t rc_get(struct rc *rc, uint32_t channel) {
	if (rc->channel[channel].corret) {
		return rc->channel[channel].value;
	} else {
		return 0;
	}
}
