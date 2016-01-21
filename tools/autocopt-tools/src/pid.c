#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <autocopt/autocopt.h>
/*
 * ARGP
 */
#include <argp.h>

/**
 * Programm Agumente
 */
struct arguments {
	struct autocopt_pidValues rate;
	struct autocopt_pidValues att;
	uint8_t mode;
};
/**
 * Setting Program Version
 */
const char *argp_program_version = PACKAGE_VERSION;

/**
 * Program Info
 */
static char doc[] = "PID Contoll Tool Options: ";
static struct argp_option options[] = { 
	{"rate-rp-p", 'a', "<value>", 0,"Roll / Pitch proportional value in Rate mode", 0}, 
	{"rate-rp-i", 'b', "<value>", 0,"Roll / Pitch integral value in Rate mode", 0}, 
	{"rate-rp-d", 'c', "<value>", 0,"Roll / Pitch derivative value in Rate mode", 0}, 
	{"rate-y-p", 'd', "<value>", 0,"Yaw proportional value in Rate mode", 0}, 
	{"rate-y-i", 'e', "<value>", 0,"Yaw integral value in Rate mode", 0}, 
	{"rate-y-d", 'f', "<value>", 0,"Yaw derivative value in Rate mode", 0}, 
	{"alt-rp-p", 'g', "<value>", 0,"Roll / Pitch proportional value in altitude mode", 0}, 
	{"alt-rp-i", 'h', "<value>", 0,"Roll / Pitch integral value in altitude mode", 0}, 
	{"alt-rp-d", 'i', "<value>", 0,"Roll / Pitch derivative value in altitude mode", 0}, 
	{"alt-y-p", 'j', "<value>", 0,"Yaw proportional value in altitude mode", 0}, 
	{"atl-y-i", 'k', "<value>", 0,"Yaw integral value in altitude mode", 0}, 
	{"atl-y-d", 'l', "<value>", 0,"Yaw derivative value in altitude mode", 0}, 
	{"roll-mode", 'm', "<rate,alt>", 0,"Set Roll in rate or alt mode", 0}, 
	{"pitch-mode", 'n', "<rate,alt>", 0,"Set Pitch in rate or alt mode", 0}, 
	{"yaw-mode", 'o', "<rate,alt>", 0,"Set Yaw in rate or alt mode", 0}, 

	{"rate-r-p", 'p', "<value>", 0,"Roll proportional value in Rate mode", 0}, 
	{"rate-r-i", 'q', "<value>", 0,"Roll integral value in Rate mode", 0}, 
	{"rate-r-d", 'r', "<value>", 0,"Roll derivative value in Rate mode", 0}, 
	{"rate-p-p", 's', "<value>", 0,"Pitch proportional value in Rate mode", 0}, 
	{"rate-p-i", 't', "<value>", 0,"Pitch integral value in Rate mode", 0}, 
	{"rate-p-d", 'u', "<value>", 0,"Pitch derivative value in Rate mode", 0}, 
	{"alt-r-p", 'v', "<value>", 0,"Roll proportional value in altitude mode", 0}, 
	{"alt-r-i", 'w', "<value>", 0,"Roll integral value in altitude mode", 0}, 
	{"alt-r-d", 'x', "<value>", 0,"Roll derivative value in altitude mode", 0}, 
	{"alt-p-p", 'y', "<value>", 0,"Pitch proportional value in altitude mode", 0}, 
	{"alt-p-i", 'z', "<value>", 0,"Pitch integral value in altitude mode", 0}, 
	{"alt-p-d", 'A', "<value>", 0,"Pitch derivative value in altitude mode", 0}, 
	{0, 0, 0, 0, 0, 0}
};
/**
 * Parse Function for argp_parse
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state);

/**
 * argp_parse Options
 */
static struct argp argp = {options, parse_opt, 0, doc, 0, 0, 0};
static void parse(char *arg, uint16_t *v, struct argp_state *state) {
	int ret;
	float t;
	ret = sscanf(arg, "%f", &t);
	if (ret <= 0) {
		fprintf(stderr, "Wrong Type of Arguemtn: %s\n", arg);
		argp_usage(state);
	}
	*v = (uint16_t) (t * 100.);
}
/**
 * ARGP Parser
 * \param key Key
 * \param arg Arguemnt
 * \param state Argumens Struct
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *args = state->input;
	switch (key) {
		case 'a':
			parse(arg, &args->rate.KpR, state);
			parse(arg, &args->rate.KpP, state);
			break;
		case 'b':
			parse(arg, &args->rate.KiR, state);
			parse(arg, &args->rate.KiP, state);
			break;
		case 'c':
			parse(arg, &args->rate.KdR, state);
			parse(arg, &args->rate.KdP, state);
			break;
		case 'd':
			parse(arg, &args->rate.KpY, state);
			break;
		case 'e':
			parse(arg, &args->rate.KiY, state);
			break;
		case 'f':
			parse(arg, &args->rate.KdY, state);
			break;
		case 'g':
			parse(arg, &args->att.KpR, state);
			parse(arg, &args->att.KpP, state);
			break;
		case 'h':
			parse(arg, &args->att.KiR, state);
			parse(arg, &args->att.KiP, state);
			break;
		case 'i':
			parse(arg, &args->att.KdR, state);
			parse(arg, &args->att.KdP, state);
			break;
		case 'j':
			parse(arg, &args->att.KpY, state);
			break;
		case 'k':
			parse(arg, &args->att.KiY, state);
			break;
		case 'l':
			parse(arg, &args->att.KdY, state);
			break;
		case 'm':
			if (strcmp(arg, "rate") == 0) {
				args->mode &= ~AUTOCOPT_PID_ANGLE_MODE_R;
			} else if (strcmp(arg, "angel") == 0) {
				args->mode |= AUTOCOPT_PID_ANGLE_MODE_R;
			} else {
				fprintf(stderr, "Mode shall rate or angel");
				argp_usage(state);
			}
			break;
		case 'n':
			if (strcmp(arg, "rate") == 0) {
				args->mode &= ~AUTOCOPT_PID_ANGLE_MODE_P;
			} else if (strcmp(arg, "angel") == 0) {
				args->mode |= AUTOCOPT_PID_ANGLE_MODE_P;
			} else {
				fprintf(stderr, "Mode shall rate or angel");
				argp_usage(state);
			}
			break;
		case 'o':
			if (strcmp(arg, "rate") == 0) {
				args->mode &= ~AUTOCOPT_PID_ANGLE_MODE_Y;
			} else if (strcmp(arg, "angel") == 0) {
				args->mode |= AUTOCOPT_PID_ANGLE_MODE_Y;
			} else {
				fprintf(stderr, "Mode shall rate or angel");
				argp_usage(state);
			}
			break;
		case 'p':
			parse(arg, &args->rate.KpR, state);
			break;
		case 'q':
			parse(arg, &args->rate.KiR, state);
			break;
		case 'r':
			parse(arg, &args->rate.KdR, state);
			break;
		case 's':
			parse(arg, &args->rate.KpP, state);
			break;
		case 't':
			parse(arg, &args->rate.KiP, state);
			break;
		case 'u':
			parse(arg, &args->rate.KdP, state);
			break;
		case 'v':
			parse(arg, &args->att.KpR, state);
			break;
		case 'w':
			parse(arg, &args->att.KiR, state);
			break;
		case 'x':
			parse(arg, &args->att.KdR, state);
			break;
		case 'y':
			parse(arg, &args->att.KpP, state);
			break;
		case 'z':
			parse(arg, &args->att.KiP, state);
			break;
		case 'A':
			parse(arg, &args->att.KdP, state);
			break;
		case ARGP_KEY_ARG:
			fprintf(stderr, "ARG: %s unknown\n", arg);
			argp_usage(state);
			/* Never executed */
		case ARGP_KEY_NO_ARGS:
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int main(int argc, char **argv) {
	int32_t ret;
	struct autocopt *copt;
	struct autocopt_msg msg = {
		.type = AUTOCOPT_TYPE_PID,
	};
	struct arguments args;
	memset(&args, 0, sizeof(struct arguments));
	/*
	 * Parsing argv
	 */
	{
		/**
		 * Setting up argp
		 */
		error_t err;
		int ret;
		ret = setenv("ARGP_HELP_FMT", "opt-doc-col=30", 1);
		if (ret < 0) {
			perror("setenv");
			exit(EXIT_FAILURE);
		}
		err = argp_parse(&argp, argc, argv, 0, 0, (void*) &args);
		if (err != 0) {
			perror("argp_parse");
			exit(EXIT_FAILURE);
		}
	}
	copt = autocopt_init(AUTOCOPT_DEFAULT_DEV);
	if (copt == NULL) {
		goto main_error0;
	}
	args.rate.header.type = AUTOCOPT_PID_TYPE_RATE;
	args.rate.header.mode = args.mode;
	printf("Send PID rate Config\n");
	ret = autocopt_pid(copt, &args.rate);
	if (ret < 0) {
		goto main_error1;
	}
	args.att.header.type = AUTOCOPT_PID_TYPE_ANGLE;
	args.att.header.mode = args.mode;
	memcpy(msg.data, &args.att, sizeof(struct autocopt_pidValues));
	printf("Send PID att Config\n");
	ret = autocopt_pid(copt, &args.att);
	if (ret < 0) {
		goto main_error1;
	}
	autocopt_deinit(copt);
	return EXIT_SUCCESS;
main_error1:
	autocopt_deinit(copt);
main_error0:
	return EXIT_FAILURE;
}
