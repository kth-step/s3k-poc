.POSIX:

BUILD?=build
S3K_PATH?=../s3k
CONFIG_H?=config.h
export RISCV_PREFIX ?=riscv64-unknown-elf-

export CC=${RISCV_PREFIX}gcc
export OBJCOPY=${RISCV_PREFIX}objcopy
export OBJDUMP=${RISCV_PREFIX}objdump

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
       -static-pie \
	-std=c11\
	-Wall -Werror\
	-g -Os\
	-flto\
	-ffreestanding\
	-Iinc

ASFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-g

LDFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-nostartfiles -static-pie -mno-relax\
	-Wl,--gc-sections,--no-dynamic-linker\
	-flto
#	-Wl,--no-warn-rwx-segments\
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
api:
	$(MAKE) -C $(S3K_PATH)/api libs3k.a
	cp $(S3K_PATH)/api/s3k.h inc/s3k.h
	cp $(S3K_PATH)/api/libs3k.a lib/libs3k.a

$(BUILD)/%.c.o: %.c
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(BUILD)/%.S.o: %.S
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(ASFLAGS) -MMD -c -o $@ $<

# Boot loader
SRCS=boot/main.c boot/capman.c boot/payload.S common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
build/boot/payload.S.o: build/monitor.bin build/crypto.bin build/uartppp.bin
$(BUILD)/boot.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^

# Monitor
SRCS=monitor/main.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/monitor.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^

# crypto
SRCS=crypto/main.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/crypto.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^

# UART driver
SRCS=uartppp/main.c uartppp/ppp.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/uartppp.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^
	
# Application 0
SRCS=app0/main.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/app0.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.lds -o $@ $^

# Application 1
SRCS=app1/main.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/app1.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
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
