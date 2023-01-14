OBJ=${addprefix obj/, ${CSRC:.c=.o} ${ASSRC:.S=.o}}
DEP=${OBJ:.o:.d}


# Compilation flags
CFLAGS+=-march=rv64imac -mabi=lp64 -mcmodel=medlow
CFLAGS+=-std=c11
CFLAGS+=-Wall -Werror
CFLAGS+=-g -Os
CFLAGS+=-flto
CFLAGS+=-static-pie
CFLAGS+=-nostdlib -ffreestanding

ASFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medlow\
	-flto\
	-g -ffreestanding

LDFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medlow\
	-nostartfiles\
	-Wl,--gc-sections,--print-gc-sections\
	-Wstack-usage=2048 -fstack-usage\
	-flto\
	-mno-relax\
	-fpie\
	-T${LDS}

obj:
	mkdir -p $@

${OBJ}: ${LDS} | obj

obj/%.o: %.c
	${CC} ${CFLAGS} ${INC} ${DEFS} -MMD -c -o $@ $<

obj/%.o: %.S
	${CC} ${ASFLAGS} ${INC} ${DEFS} -MMD -c -o $@ $<

${PROGRAM}.elf: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

${PROGRAM}.bin: ${PROGRAM}.elf
	${OBJCOPY} -O binary $< $@

-include ${DEP}
