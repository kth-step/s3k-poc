.POSIX:

# Prefix of toolchain
export RISCV_PREFIX?=riscv64-unknown-elf
#export CFLAGS+=-DNDEBUG

# Kernel configuration file
export S3K_CONFIG_H=config.h

# Subdirectories
ROOT=$(abspath .)
PROGRAMS=separation-kernel monitor uart_in uart_out app0 app1
BINS=$(join $(PROGRAMS), $(patsubst %, /%.bin, $(PROGRAMS)))
ELFS=$(join $(PROGRAMS), $(patsubst %, /%.elf, $(PROGRAMS)))
DAS =$(join $(PROGRAMS), $(patsubst %, /%.da, $(PROGRAMS)))

TARGET=$(KERNEL_ELF)
KERNEL_ELF=$(filter %separation-kernel.elf, $(ELFS))
MONITOR_ELF=$(filter %monitor.elf, $(ELFS))
MONITOR_BIN=$(filter %monitor.bin, $(BINS))
APP_BINS=$(filter-out %monitor.bin %kernel.bin, $(BINS))

SIZE=$(RISCV_PREFIX)-size
OBJCOPY=$(RISCV_PREFIX)-objcopy

.PHONY: all clean format size api $(CLEAN) qemu $(ELFS) $(BINS) $(DAS)
.SECONDARY:

all: $(TARGET)

api:
	$(MAKE) -C separation-kernel api

$(APP_BINS): api
	$(MAKE) -C $(basename $(notdir $@)) bin

$(MONITOR_BIN): $(APP_BINS) api
	$(MAKE) -C $(basename $(notdir $@)) PAYLOADS="$(abspath $(APP_BINS))" bin 

$(KERNEL_ELF): $(MONITOR_BIN) $(S3K_CONFIG_H)
	$(MAKE) -C $(basename $(notdir $@)) \
		CONFIG_H="$(abspath $(S3K_CONFIG_H))" \
		PAYLOAD="$(abspath $(MONITOR_BIN))" elf

$(DAS):
	$(MAKE) -C $(basename $(notdir $@)) da

clean: 
	for i in $(PROGRAMS); do \
		$(MAKE) -C $$i clean; \
	done

format:
	for i in $(PROGRAMS); do \
		$(MAKE) -C $$i format; \
	done

size: $(ELFS)
	$(SIZE) $(ELFS)


qemu: $(KERNEL_ELF) $(MONITOR_ELF)
	./scripts/qemu.sh $(KERNEL_ELF) $(MONITOR_ELF)
