PROGRAM=uart
INC=-I../common

vpath %.c ../common
vpath %.S ../common

CSRC=main.c ppp.c s3k.c
ASSRC=start.S common.S
LDS=../default.lds
BASE_ADDR=0x80016000

include ../config.mk
