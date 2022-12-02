.POSIX:
.PHONY: all clean qemu
.SECONDARY:

S3K_PATH=../s3k
BSP=$(S3K_PATH)/bsp/virt

BUILDDIR=build
S3K=$(BUILDDIR)/s3k.elf
MONITOR=$(BUILDDIR)/monitor.elf
APPS=$(patsubst %, $(BUILDDIR)/%.bin, app0 app1 uart_in uart_out)

LDS=default.lds
CONFIG_H=config.h

API=$(S3K_PATH)/api
API_H=s3k.h s3k_consts.g.h s3k_cap.g.h

RISCV_PREFIX?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
OBJCOPY=$(RISCV_PREFIX)objcopy
OBJDUMP=$(RISCV_PREFIX)objdump
SIZE=$(RISCV_PREFIX)size

CFLAGS+=-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-std=c18
CFLAGS+=-Wall
CFLAGS+=-O3 -gdwarf-2
CFLAGS+=-I$(API) -Icommon -I$(BSP) -I../common
CFLAGS+=-T$(LDS) -nostartfiles -ffreestanding -mno-relax -static-pie

all: $(S3K) $(MONITOR)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

$(BUILDDIR)/%.elf: %/*.[cS] common/*.[cS] api
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $(filter-out api, $^)

$(MONITOR): monitor/*.[cS] common/*.[cS] api $(APPS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $(filter-out api %.bin, $^)

$(S3K):
	@mkdir -p $(BUILDDIR)
	$(MAKE) -C $(S3K_PATH) $(abspath $(S3K)) \
		CONFIG_H=$(abspath $(CONFIG_H)) BSP=$(abspath $(BSP))

api:
	$(MAKE) -C $(S3K_PATH) api

qemu: $(S3K) $(MONITOR)
	./scripts/qemu.sh $(S3K) $(MONITOR)

clean:
	$(MAKE) -C $(S3K_PATH) clean
	rm -f build/*.elf build/*.bin
