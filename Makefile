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
	rm -rf $(BUILD)

# API
api: inc/s3k.h lib/libs3k.a

inc/s3k.h: $(S3K_PATH)/api/s3k.h
	cp $(S3K_PATH)/api/s3k.h inc/s3k.h

lib/libs3k.a: $(wildcard $(S3K_PATH)/api/*.c) inc/s3k.h
	$(MAKE) -C $(S3K_PATH)/api libs3k.a
	cp $(S3K_PATH)/api/libs3k.a lib/libs3k.a

qemu: $(BUILD)/s3k.elf $(BUILD)/boot.elf
	./scripts/qemu.sh $^

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
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# Monitor
SRCS=monitor/main.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/monitor.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# crypto
SRCS=crypto/main.c crypto/aes128.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/crypto.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# UART driver
SRCS=uartppp/main.c uartppp/ppp.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/uartppp.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^
	
# Application 0
SRCS=app0/main.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/app0.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# Application 1
SRCS=app1/main.c common/start.S
DEPS+=$(patsubst %, $(BUILD)/%.d, $(SRCS))
$(BUILD)/app1.elf: $(patsubst %, $(BUILD)/%.o, $(SRCS)) lib/libs3k.a
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# Make kernel
$(BUILD)/s3k.elf: ${CONFIG_H}
	@mkdir -p ${@D}
	@${MAKE} -C ${S3K_PATH} options kernel  \
		CONFIG_H=${abspath ${CONFIG_H}} \
		BUILD_DIR=$(abspath $(BUILD))   \
		OBJ_DIR=$(abspath $(BUILD))/s3k

# Create bin file from elf
%.bin: %.elf
	@printf "OBJCOPY $< $@\n"
	@${OBJCOPY} -O binary $< $@

# Create assebly dump
%.da: %.elf
	@printf "OBJDUMP $< $@\n"
	@${OBJDUMP} -d $< > $@

.PHONY: all api clean qemu s3k.elf

-include $(DEPS)
