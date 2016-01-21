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
#include <ncurses.h>

#define PITCH_STEPSIZE 0.01
#define ROLL_SETPSIZE 0.01
#define YAW_SETPSIZE 0.01
#define THURST_STEPSIZE 1000

int startx;
int starty; 
int width;
int height;
WINDOW *win;
WINDOW *winBox;


struct autocopt_control ctl = {
	0,0,0,0
};
struct autocopt_control ctlSave = {
	0,0,0,0
};

static void updateWin() {
	mvwprintw(win, 1, 0, "roll:%f", ctl.roll);
	mvwprintw(win, 2, 0, "pitch:%f", ctl.pitch);
	mvwprintw(win, 3, 0, "yaw:%f", ctl.yaw);
	mvwprintw(win, 4, 0, "thrust:%u", ctl.thrust);
	box(winBox, 0 , 0);
	wnoutrefresh(winBox);
	wnoutrefresh(win);
	doupdate();
}

static int32_t initScreen() {
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	
	win = newwin(width - 2, height - 1, starty + 1 , startx + 2);
	winBox = newwin(width, height, starty, startx);
	updateWin();
	updateWin();
	return 0;
}

int main(int argc, char **argv) {
	struct autocopt *copt;
	int ch;
	int32_t ret;
	copt = autocopt_init(AUTOCOPT_DEFAULT_DEV);
	if (copt == NULL) {
		return EXIT_FAILURE;
	}
	ret = autocopt_select(copt, AUTOCOPT_MODE_LINUX);
	if (ret < 0) {
		fprintf(stderr, "Can't select mode\n");
		return EXIT_FAILURE;
	}
	initscr();
	height = 30;
	width = 30;
	starty = (LINES - height) / 2;
	startx = (COLS - width) / 2;
	initScreen();
	while((ch = getch()) != 'z') {
		switch(ch) {
			case 'w':
				ctl.pitch += PITCH_STEPSIZE;
				if (ctl.pitch > 1.) {
					ctl.pitch = 1;
				}
				break;
			case 's':
				ctl.pitch -= PITCH_STEPSIZE;
				if (ctl.pitch < -1.) {
					ctl.pitch = -1;
				}
				break;
			case 'd':
				ctl.roll += ROLL_SETPSIZE;
				if (ctl.roll > 1.) {
					ctl.roll = 1;
				}
				break;
			case 'a':
				ctl.roll -= ROLL_SETPSIZE;
				if (ctl.roll < -1.) {
					ctl.roll = -1;
				}
				break;
			case 'q':
				ctl.yaw -= YAW_SETPSIZE;
				if (ctl.yaw < -1.) {
					ctl.yaw = -1;
				}
				break;
			case 'e':
				ctl.yaw += YAW_SETPSIZE;
				if (ctl.yaw > 1.) {
					ctl.yaw = 1;
				}
				break;
			case 'f':
				if ((((int32_t) ctl.thrust) - THURST_STEPSIZE) < 0) {
					ctl.thrust = 0;
					break;
				}
				ctl.thrust -= THURST_STEPSIZE;
				break;
			case 'r':
				if ((((int32_t) ctl.thrust) + THURST_STEPSIZE) > UINT16_MAX) {
					ctl.thrust = UINT16_MAX;
					break;
				}
				ctl.thrust += THURST_STEPSIZE;
				break;
			case 't':
				ctl.roll = ctlSave.roll;
				ctl.pitch = ctlSave.pitch;
				ctl.yaw = ctlSave.yaw;
				break;
			case 'g':
				ctlSave.roll = ctl.roll;
				ctlSave.pitch = ctl.pitch;
				ctlSave.yaw = ctl.yaw;
				break;
			case 'z':
				break;
			default:
				mvwprintw(win, 0, 0, "err: %c", ch);
				break;
		}
		ret = autocopt_control(copt, &ctl);
		if (ret < 0) {
			mvwprintw(win, 0, 0, "can't send to copter");
		}
		updateWin();
	}
	ret = autocopt_select(copt, AUTOCOPT_MODE_SPECTRUM);
	if (ret < 0) {
		fprintf(stderr, "Can't select mode\n");
		return EXIT_FAILURE;
	}
	endwin();

	return EXIT_SUCCESS;
}
