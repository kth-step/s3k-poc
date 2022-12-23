.POSIX:
.PHONY: all clean qemu build/s3k.elf
.SECONDARY:


# Tools
RISCV_PREFIX ?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
AS=$(RISCV_PREFIX)as
OBJCOPY=$(RISCV_PREFIX)objcopy
OBJDUMP=$(RISCV_PREFIX)objdump


# Compilation flags
CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medlow
CFLAGS+=-std=c18
CFLAGS+=-Wall -Werror
CFLAGS+=-g -Os 
CFLAGS+=-fPIE -static-pie -Wl,--no-dynamic-linker
CFLAGS+=-nostartfiles -ffreestanding
CFLAGS+=-Icommon/inc
CFLAGS+=-Tdefault.lds


objs_of=$(patsubst %.c, build/%.o, $(filter %.c, $(1))) \
	$(patsubst %.S, build/%.o, $(filter %.S, $(1)))


MONI_SRC=monitor/main.c common/start.S \
	 monitor/payloads/app0.c monitor/payloads/app1.c monitor/payloads/uart.c
MONI_OBJ=$(call objs_of, $(MONI_SRC))

UART_SRC=uart/main.c common/start.S uart/ppp.c common/printf.c
UART_OBJ=$(call objs_of, $(UART_SRC))

APP0_SRC=app0/main.c common/start.S
APP0_OBJ=$(call objs_of, $(APP0_SRC))

APP1_SRC=app1/main.c common/start.S
APP1_OBJ=$(call objs_of, $(APP1_SRC))


all: build/s3k.elf build/moni.elf build/app0.bin build/app1.bin

clean:
	rm -fr build/ monitor/payloads/

qemu: build/s3k.elf build/moni.elf
	./scripts/qemu.sh $^


# ELF dependencies
build/moni.elf: $(MONI_OBJ) default.lds
build/uart.elf: $(UART_OBJ) default.lds
build/app0.elf: $(APP0_OBJ) default.lds
build/app1.elf: $(APP1_OBJ) default.lds

# Kernel build rule 
build/s3k.elf:
	$(MAKE) -C ../s3k BUILD=$(abspath build) CONFIG_H=$(abspath config.h)


build/%.elf:
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -o $@ $(filter %.o, $^)

build/%.bin: build/%.elf
	@mkdir -p $(@D)
	$(OBJCOPY) -O binary $< $@

monitor/payloads/%.c: build/%.bin
	@mkdir -p $(@D)
	xxd -i -n $(notdir $<) $< > $@

build/%.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -c -o $@ $<

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -c -o $@ $<
