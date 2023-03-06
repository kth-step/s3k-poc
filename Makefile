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
	cp $< $@

common/s3k-utils.c: ../s3k/api/s3k-utils.c
	cp $< $@

common/s3k-syscall.c: ../s3k/api/s3k-syscall.c
	cp $< $@

api: common/s3k.h common/s3k-utils.c common/s3k-syscall.c

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
