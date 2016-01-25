#include <stdio.h>
#include <commander.h>
#include <rc_commander.h>
#include <linux_client.h>
#include <linux_commander.h>
#include <string.h>

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
};

struct linuxComm comm = {
	.ctl = {0,0,0-1},
};


static void lc_control(struct lc *lc, struct lc_msg *msg);
static void lc_select(struct lc *lc, struct lc_msg *msg);
struct linuxComm *linuxComm_init(struct lc *lc) {
	if (comm.init) {
		return &comm;
	}
	comm.init = true;
	comm.lc = lc;
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
	memcpy(&comm.ctl, msg->data, sizeof(struct autocopt_control));
	lc_sendAct(lc);
}
static void linuxComm_GetThrust(uint16_t* thrust) {
	float t;
	t = comm.ctl.thrust;
	t += 1;
	t /= 2;

	*thrust = t * UINT16_MAX;
}

static void linuxComm_GetRPY(float* eulerRollDesired, float* eulerPitchDesired, float* eulerYawDesired) {
#if 1
	float roll;
	float pitch;
	float yaw;
	roll = comm.ctl.roll;
	pitch = -comm.ctl.pitch;
	yaw = comm.ctl.yaw;

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
	switch (msg->data[0]) {
#ifdef CONFIG_RC
		case 0: /* TODO Move to DEFINE */
			rcComm_selectThrust();
			rcComm_selectRTY();
			break;
#endif
		case 1:
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
