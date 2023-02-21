OBJ=${addprefix obj/, ${CSRC:.c=.o} ${ASSRC:.S=.o}}
DEP=${OBJ:.o:.d}

# Compilation flags
CFLAGS+=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-std=c11\
	-Wall -Werror\
	-g -Os\
	-flto\
	-ffreestanding

ASFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-g\
	-flto\
	-ffreestanding

LDFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-nostdlib -static-pie -mno-relax\
	-Wl,--gc-sections,--no-dynamic-linker\
	-Wstack-usage=2048 -fstack-usage\
	-flto\
	-T${LDS}

obj:
	mkdir -p $@

${OBJ}: ${LDS} | obj

obj/%.o: %.c
	${CC} ${CFLAGS} ${INC} ${DEFS} -MMD -c -o $@ $<

obj/%.o: %.S
	${CC} ${ASFLAGS} ${INC} ${DEFS} -MMD -c -o $@ $<

${PROGRAM}.elf: ${OBJ} ${LDS}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

${PROGRAM}.bin: ${PROGRAM}.elf
	${OBJCOPY} -O binary $< $@

-include ${DEP}
