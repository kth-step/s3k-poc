.POSIX:

# Prefix of toolchain
export RISCV_PREFIX?=riscv64-unknown-elf
#export CFLAGS+=-DNDEBUG

# Kernel configuration file
export S3K_CONFIG_H=config.h

# Subdirectories
ROOT=$(abspath .)
PROGRAMS=separation-kernel init uart monitor
BINS=$(join $(PROGRAMS), $(patsubst %, /build/%.bin, $(PROGRAMS)))
ELFS=$(join $(PROGRAMS), $(patsubst %, /build/%.elf, $(PROGRAMS)))
DAS =$(join $(PROGRAMS), $(patsubst %, /build/%.da, $(PROGRAMS)))

TARGET=$(KERNEL_ELF)
KERNEL_ELF=$(filter %separation-kernel.elf, $(ELFS))
INIT_BIN=$(filter %init.bin, $(BINS))
APP_BINS=$(filter-out %init.bin %kernel.bin, $(BINS))

CLEAN=$(addsuffix .clean, $(PROGRAMS))

.PHONY: all clean da api $(CLEAN) qemu $(ELFS) $(BINS) $(DAS)
.SECONDARY:


all: $(TARGET)

api:
	$(MAKE) -C separation-kernel api

$(APP_BINS): api
	$(MAKE) -C $(basename $(notdir $@)) bin

$(INIT_BIN): $(APP_BINS) api
	$(MAKE) -C $(basename $(notdir $@)) bin 

$(KERNEL_ELF): $(INIT_BIN) $(S3K_CONFIG_H)
	$(MAKE) -C $(basename $(notdir $@)) PAYLOAD=$(abspath $(INIT_BIN)) elf

$(DAS):
	$(MAKE) -C $(basename $(notdir $@)) PAYLOAD=$(abspath $(INIT_BIN)) da

clean: $(CLEAN)

$(CLEAN):
	$(MAKE) -C $(basename $@) clean

da: $(DAS)

qemu: $(KERNEL_ELF)
	./scripts/qemu.sh $(KERNEL_ELF)
