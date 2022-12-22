.POSIX:
.PHONY: all clean qemu build/s3k.elf

RISCV_PREFIX?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
AS=$(RISCV_PREFIX)as
OBJCOPY=$(RISCV_PREFIX)objcopy
OBJDUMP=$(RISCV_PREFIX)objdump

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medlow
CFLAGS+=-std=c17 -MMD
CFLAGS+=-g -O3 -fPIE -static -Wl,--no-dynamic-linker
CFLAGS+=-nostartfiles -nostdlib -Icommon/inc -ffreestanding
CFLAGS+=-Tdefault.lds

MONITOR_SRCS=monitor/main.c monitor/payload.S common/start.S common/printf.c
MONITOR_OBJS=$(patsubst %, build/%.o, $(MONITOR_SRCS))
MONITOR_DEPS=$(MONITOR_OBJS:.o=.d)

UART_SRCS=uart/main.c common/start.S uart/ppp.c common/printf.c
UART_OBJS=$(patsubst %, build/%.o, $(UART_SRCS))
UART_DEPS=$(UART_OBJS:.o=.d)

APP0_SRCS=app0/main.c common/start.S
APP0_OBJS=$(patsubst %, build/%.o, $(APP0_SRCS))
APP0_DEPS=$(APP0_OBJS:.o=.d)

APP1_SRCS=app1/main.c common/start.S
APP1_OBJS=$(patsubst %, build/%.o, $(APP1_SRCS))
APP1_DEPS=$(APP1_OBJS:.o=.d)

PAYLOAD_APPS=uart app0 app1
PAYLOAD_HEXS=$(patsubst %, build/%.hex, $(PAYLOAD_APPS))
PAYLOAD_BINS=$(patsubst %, build/%.bin, $(PAYLOAD_APPS))
PAYLOAD_DAS=$(patsubst %, build/%.da, $(PAYLOAD_APPS))


all: build/s3k.elf build/monitor.elf build/s3k.da build/monitor.da $(PAYLOAD_HEXS)
clean:
	rm -fr build

qemu: build/s3k.elf build/monitor.elf
	./scripts/qemu.sh $^

build/monitor.elf: $(MONITOR_OBJS) default.lds
build/uart.elf: $(UART_OBJS) default.lds
build/app0.elf: $(APP0_OBJS) default.lds
build/app1.elf: $(APP1_OBJS) default.lds

build/s3k.elf:
	$(MAKE) -C ../s3k BUILD=$(abspath build) CONFIG_H=$(abspath config.h)

%.elf:
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -o $@ $(filter %.o, $^)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.da: %.elf
	$(OBJDUMP) -d $< > $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

build/monitor/payload.S.o: monitor/payload.S $(PAYLOAD_BINS)

build/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -c -o $@ $<

build/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -c -o $@ $<

-include $(MONITOR_DEPS) $(UART_DEPS) $(APP0_DEPS) $(APP1_DEPS)
