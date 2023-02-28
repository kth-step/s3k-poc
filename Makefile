.POSIX:

RISCV_PREFIX ?=riscv64-unknown-elf-
export CC=${RISCV_PREFIX}gcc
export OBJCOPY=${RISCV_PREFIX}objcopy
export OBJDUMP=${RISCV_PREFIX}objdump

MONITOR=monitor/monitor.elf
KERNEL=../s3k/s3k.elf
CONFIG_H=${abspath config.h}

all: ${MONITOR} ${KERNEL}

clean:
	git clean -fdX
	${MAKE} -C ${dir ${KERNEL}} clean

common/s3k.h: ../s3k/api/s3k.h
	cp ../s3k/api/s3k.h common/s3k.h

common/s3k.c: ../s3k/api/s3k.c
	cp ../s3k/api/s3k.c common/s3k.c

api: common/s3k.h common/s3k.c

${MONITOR}: api
	${MAKE} -C ${@D} ${@F}

${KERNEL}:
	${MAKE} -C ${@D} ${@F} CONFIG_H=${CONFIG_H}

%.bin: %.elf
	${OBJCOPY} -O binary $< $@

%.da: %.elf
	${OBJDUMP} -d $< > $@

qemu: $(KERNEL) $(MONITOR)
	./scripts/qemu.sh $(KERNEL) $(MONITOR)

.PHONY: all api clean qemu ${MONITOR} ${KERNEL}
