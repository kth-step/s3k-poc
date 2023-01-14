.POSIX:

RISCV_PREFIX ?=riscv64-unknown-elf-
export CC=${RISCV_PREFIX}gcc
export OBJCOPY=${RISCV_PREFIX}objcopy
export OBJDUMP=${RISCV_PREFIX}objdump

MONITOR=monitor/monitor.elf
KERNEL=../s3k/s3k.elf

all: ${MONITOR} ${KERNEL}

clean:
	git clean -fdX
	${MAKE} -C ${dir ${KERNEL}} clean

${MONITOR}:
	${MAKE} -C ${@D} ${@F}

${KERNEL}:
	${MAKE} -C ${@D} ${@F}

%.bin: %.elf
	${OBJCOPY} -O binary $< $@

%.da: %.elf
	${OBJDUMP} -d $< > $@

.PHONY: all clean qemu ${MONITOR} ${KERNEL}
