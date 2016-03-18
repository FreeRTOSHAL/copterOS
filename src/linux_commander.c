#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <commander.h>
#include <rc_commander.h>
#include <linux_client.h>
#include <linux_commander.h>
#include <string.h>

#define MAX_DIFF 0.5

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
	 * -1 = no Thrust
	 * 1 = full Thrust
	 * \waring Value below 90 % the copter may instable 
	 */
	float thrust;
} __attribute__((packed));

struct linuxComm {
	bool init;
	struct lc *lc;
	struct autocopt_control ctl;
#ifdef CONFIG_LC_COMMANDER_DETECT_BIG_DIFF
	struct autocopt_control oldctl;
#endif
	SemaphoreHandle_t mutex;
};

struct linuxComm comm = {
	.ctl = {0,0,0,-1},
#ifdef CONFIG_LC_COMMANDER_DETECT_BIG_DIFF
	.oldctl = {0,0,0,-1},
#endif
};


static void lc_control(struct lc *lc, struct lc_msg *msg);
static void lc_select(struct lc *lc, struct lc_msg *msg);
struct linuxComm *linuxComm_init(struct lc *lc) {
	if (comm.init) {
		return &comm;
	}
	comm.lc = lc;
	comm.mutex = xSemaphoreCreateMutex();
	if (comm.mutex == NULL) {
		return NULL;
	}
	comm.init = true;
	lc_registerCallback(lc, LC_TYPE_SELECT, lc_select);
	lc_registerCallback(lc, LC_TYPE_CONTROL, lc_control);
#ifdef CONFIG_DEFAULT_RC_LINUX
	linuxComm_selectThrust();
	linuxComm_selectRTY();
#endif
	return &comm;
}
int32_t linuxComm_deinit() {
	return 0;
}

static void lc_control(struct lc *lc, struct lc_msg *msg) {
	struct autocopt_control *copt = (struct autocopt_control *) msg->data;
	if (
		copt->roll < -1 || copt->roll > 1 ||
		copt->pitch < -1 || copt->pitch > 1 ||
		copt->yaw < -1 || copt->yaw > 1 ||
		copt->thrust < -1 || copt->thrust > 1
	) {
		printf("No correct Values in struct roll: %f pitch: %f yaw: %f thrust: %f\n", copt->roll, copt->pitch, copt->yaw, copt->thrust);
		lc_sendFailt(lc);
		return;
	}
	xSemaphoreTake(comm.mutex, portMAX_DELAY);
#ifdef CONFIG_LC_COMMANDER_DETECT_BIG_DIFF
	memcpy(&comm.oldctl, &comm.ctl, sizeof(struct autocopt_control));
#endif
	memcpy(&comm.ctl, msg->data, sizeof(struct autocopt_control));
	xSemaphoreGive(comm.mutex);
	lc_sendAct(lc);

#ifdef CONFIG_LC_COMMANDER_DETECT_BIG_DIFF
	{
		TickType_t time = xTaskGetTickCount();
		if (((comm.oldctl.roll - comm.ctl.roll) > MAX_DIFF) || ((comm.oldctl.roll - comm.ctl.roll) < -MAX_DIFF)) {
			printf("[%08lu] roll big? diff: %f\n", time, copt->roll - comm.oldctl.roll);
			printf("[%08lu] roll big? roll: %f\n", time, copt->roll);
			printf("[%08lu] roll big? old roll: %f\n", time, comm.oldctl.roll);
		}
		if (((comm.oldctl.pitch - comm.ctl.pitch) > MAX_DIFF) || ((comm.oldctl.pitch - comm.ctl.pitch) < -MAX_DIFF)) {
			printf("[%08lu] pitch big? diff: %f\n", time, copt->pitch - comm.oldctl.pitch);
			printf("[%08lu] pitch big? pitch: %f\n", time, copt->pitch);
			printf("[%08lu] pitch big? old pitch: %f\n", time, comm.oldctl.pitch);
		}
		if (((comm.oldctl.yaw - comm.ctl.yaw) > MAX_DIFF) || ((comm.oldctl.yaw - comm.ctl.yaw) < -MAX_DIFF)) {
			printf("[%08lu] yaw big? diff: %f\n", time, copt->yaw - comm.oldctl.yaw);
			printf("[%08lu] yaw big? yaw: %f\n", time, copt->yaw);
			printf("[%08lu] yaw big? old yaw: %f\n", time, comm.oldctl.yaw);
		}
		if (((comm.oldctl.thrust - comm.ctl.thrust) > MAX_DIFF) || ((comm.oldctl.thrust - comm.ctl.thrust) < -MAX_DIFF)) {
			printf("[%08lu] thrust big? diff: %f\n", time, copt->thrust - comm.oldctl.thrust);
			printf("[%08lu] thrust big? thrust: %f\n", time, copt->thrust);
			printf("[%08lu] thrust big? old thrust: %f\n", time, comm.oldctl.thrust);
		}
	}
#endif
}
static void linuxComm_GetThrust(uint16_t* thrust) {
	float t;
	xSemaphoreTake(comm.mutex, portMAX_DELAY);
	t = comm.ctl.thrust;
	xSemaphoreGive(comm.mutex);

	t += 1;
	t /= 2;

	*thrust = t * UINT16_MAX;
}

static void linuxComm_GetRPY(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired) {
#if 1
	float roll;
	float pitch;
	float yaw;
	xSemaphoreTake(comm.mutex, portMAX_DELAY);
	roll = comm.ctl.roll;
	pitch = -comm.ctl.pitch;
	yaw = comm.ctl.yaw;
	xSemaphoreGive(comm.mutex);

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

void linuxComm_selectThrust() {
	commanderSetThrust(&linuxComm_GetThrust);
}
void linuxComm_selectRTY() {
	commanderSetRPY(&linuxComm_GetRPY);
}

static void lc_select(struct lc *lc, struct lc_msg *msg) {
	TickType_t time = xTaskGetTickCount();
	switch (msg->data[0]) {
#ifdef CONFIG_RC
		case 0: /* TODO Move to DEFINE */
			printf("[%08lu] Select RC\n", time);
			rcComm_selectThrust();
			rcComm_selectRTY();
			break;
#endif
		case 1:
			printf("[%08lu] Select Linux Cilent\n", time);
			linuxComm_selectThrust();
			linuxComm_selectRTY();
			break;
#if defined(CONFIG_RC) && defined(CONFIG_LC)
		case 2:
			linuxComm_selectThrust();
			rcComm_selectRTY();
			break;
		case 3:
			rcComm_selectThrust();
			linuxComm_selectRTY();
			break;
#endif
		default:
			lc_sendFailt(lc);
			return;
	}
	lc_sendAct(lc);
}
