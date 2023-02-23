PROGRAM=monitor
INC=-I../common

vpath %.c ../common
vpath %.S ../common

CSRC=main.c s3k.c
ASSRC=start.S payload.S common.S
LDS=../monitor.lds
BASE_ADDR=0x80010000

BIN=../app0/app0.bin ../app1/app1.bin ../uart/uart.bin

${BIN}:
	${MAKE} -C ${@D} ${@F}

obj/payload.o: ${BIN}

include ../config.mk

.PHONY: ${BIN}

