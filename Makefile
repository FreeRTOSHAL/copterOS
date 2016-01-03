PROJECT_NAME    := copter
objs-y		:= src arch mach driver freertos
#libs-y          := libs
KBUILD_CLFLAGS := -Wall

include scripts/Makefile.project

