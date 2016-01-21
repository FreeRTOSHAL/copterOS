PROJECT_NAME             := copter
objs-y		         := src arch mach driver freertos crazyflie
#libs-y                  := libs
KBUILD_CLFLAGS := -Wall

include scripts/Makefile.project
