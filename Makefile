.POSIX:

BUILD?=build
S3K_PATH?=../s3k
CONFIG_H?=config.h
RISCV_PREFIX ?=riscv64-unknown-elf-

export CC=${RISCV_PREFIX}gcc
export OBJCOPY=${RISCV_PREFIX}objcopy
export OBJDUMP=${RISCV_PREFIX}objdump

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-std=c11\
	-Wall -Werror\
	-g -Os\
	-flto\
	-ffreestanding\
	-Icommon

ASFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-g

LDFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-nostdlib -static-pie -mno-relax\
	-Wl,--gc-sections,--no-dynamic-linker\
	-Wl,--no-warn-rwx-segments\
	-flto
#	-Wstack-usage=2048 -fstack-usage\

# Build all
all: options $(BUILD)/boot.elf $(BUILD)/s3k.elf

options:
	@printf "build options:\n"
	@printf "CC       = ${CC}\n"
	@printf "LDFLAGS  = ${LDFLAGS}\n"
	@printf "ASFLAGS  = ${ASFLAGS}\n"
	@printf "CFLAGS   = ${CFLAGS}\n"
	@printf "S3K_PATH = ${S3K_PATH}\n"
	@printf "CONFIG_H = ${abspath ${CONFIG_H}}\n"

# Clean all
clean:
	git clean -fdX
	${MAKE} -C ${S3K_PATH} clean

# API
# TODO: Make static archive file (s3k.a)
common/s3k.h: ../s3k/api/s3k.h
	cp $< $@

common/s3k-utils.c: ../s3k/api/s3k-utils.c
	cp $< $@

common/s3k-syscall.c: ../s3k/api/s3k-syscall.c
	cp $< $@

api: common/s3k.h common/s3k-utils.c common/s3k-syscall.c

$(BUILD)/%.c.o: %.c
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(BUILD)/%.S.o: %.S
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(ASFLAGS) -MMD -c -o $@ $<

# Boot loader
SRCS=boot/main.c boot/capman.c boot/payload.S common/s3k-syscall.c common/s3k-utils.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
build/boot/payload.S.o: build/uart.bin build/app0.bin build/app1.bin
$(BUILD)/boot.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^

# UART driver
SRCS=uart/main.c uart/ppp.c common/s3k-syscall.c common/s3k-utils.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/uart.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^

# Application 0
SRCS=app0/main.c common/s3k-syscall.c common/s3k-utils.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/app0.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^

# Application 1
SRCS=app1/main.c common/s3k-syscall.c common/s3k-utils.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/app1.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^

# Make kernel
$(BUILD)/s3k.elf:
	@mkdir -p ${@D}
	@printf "\nBuilding S3K\n"
	@${MAKE} -C ${S3K_PATH} options ${abspath $@} CONFIG_H=${abspath ${CONFIG_H}}

# Create bin file from elf
%.bin: %.elf
	@printf "OBJCOPY $< $@\n"
	@${OBJCOPY} -O binary $< $@

# Create assebly dump
%.da: %.elf
	@printf "OBJDUMP $< $@\n"
	@${OBJDUMP} -d $< > $@

qemu: $(BUILD)/s3k.elf $(BUILD)/boot.elf
	./scripts/qemu.sh $(BUILD)/s3k.elf $(BUILD)/boot.elf

.PHONY: all api clean qemu s3k.elf

-include $(DEPS)
