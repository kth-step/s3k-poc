PROGRAM=boot
INC=-I../common

vpath %.c ../common
vpath %.S ../common

CSRC=main.c capman.c s3k-utils.c s3k-syscall.c
ASSRC=start.S payload.S common.S
LDS=../monitor.lds

BIN=../app0/app0.bin ../app1/app1.bin ../uart/uart.bin

${BIN}:
	${MAKE} -C ${@D} ${@F}

obj/payload.o: ${BIN}

include ../config.mk

.PHONY: ${BIN}

