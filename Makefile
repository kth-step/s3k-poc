.POSIX:

BUILD?=build
S3K_PATH?=../s3k
S3K_LIB?=${S3K_PATH}/lib
CONFIG_H?=config.h
export RISCV_PREFIX ?=riscv64-unknown-elf-

export CC=${RISCV_PREFIX}gcc
export OBJCOPY=${RISCV_PREFIX}objcopy
export OBJDUMP=${RISCV_PREFIX}objdump

ARCH=rv64imaczicsr
ABI=lp64
CMODEL=medany

CFLAGS =-march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CMODEL)\
	-static-pie \
	-std=c11\
	-Wall -Werror\
	-g -Os\
	-flto\
	-ffreestanding\
	-Iinc -I${S3K_LIB}

ASFLAGS=-march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CMODEL)\
	-g

LDFLAGS=-march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CMODEL)\
	-nostdlib -static-pie -mno-relax\
	-Wl,--gc-sections,--no-dynamic-linker\
	-flto
#	-Wl,--no-warn-rwx-segments\
#	-Wstack-usage=2048 -fstack-usage\

BOOT_SRCS=boot/main.c boot/payload.S common/start.S common/common.S
MONITOR_SRCS=monitor/main.c common/start.S common/common.S
CRYPTO_SRCS=crypto/main.c crypto/aes128.c common/start.S common/common.S
UART_SRCS=uartppp/main.c uartppp/ppp.c common/start.S common/common.S
APP0_SRCS=app0/main.c common/start.S common/common.S
APP1_SRCS=app1/main.c common/start.S common/common.S

DEPS=$(patsubst %, $(BUILD)/%.d, $(BOOT_SRCS) $(MONITOR_SRCS) $(CRYPTO_SRCS)\
                                 $(UART_SRCS) $(APP0_SRCS) $(APP1_SRCS))

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
build/boot/payload.S.o: build/monitor.bin build/crypto.bin build/uartppp.bin
$(BUILD)/boot.elf: $(patsubst %, $(BUILD)/%.o, $(BOOT_SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# Monitor
$(BUILD)/monitor.elf: $(patsubst %, $(BUILD)/%.o, $(MONITOR_SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# crypto
$(BUILD)/crypto.elf: $(patsubst %, $(BUILD)/%.o, $(CRYPTO_SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# UART driver
$(BUILD)/uartppp.elf: $(patsubst %, $(BUILD)/%.o, $(UART_SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^
	
# Application 0
$(BUILD)/app0.elf: $(patsubst %, $(BUILD)/%.o, $(APP0_SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# Application 1
$(BUILD)/app1.elf: $(patsubst %, $(BUILD)/%.o, $(APP1_SRCS))
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@$(CC) $(LDFLAGS) -Tdefault.ld -o $@ $^

# Make kernel
$(BUILD)/s3k.elf: ${CONFIG_H}
	@mkdir -p ${@D}
	@${MAKE} -C ${S3K_PATH} options kernel  \
		CONFIG_H=${abspath ${CONFIG_H}} \
		BUILD=$(abspath $(BUILD))

# Create bin file from elf
%.bin: %.elf
	@printf "OBJCOPY $< $@\n"
	@${OBJCOPY} -O binary $< $@

# Create assebly dump
%.da: %.elf
	@printf "OBJDUMP $< $@\n"
	@${OBJDUMP} -d $< > $@

.PHONY: all api clean qemu $(BUILD)/s3k.elf

-include $(DEPS)
