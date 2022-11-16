.POSIX:

# Prefix of toolchain
export RISCV_PREFIX?=riscv64-unknown-elf
#export CFLAGS+=-DNDEBUG

# Kernel configuration file
export S3K_CONFIG_H=config.h

# Subdirectories
PROGRAMS=separation-kernel root uart monitor
BINS=$(join $(PROGRAMS), $(patsubst %, /build/%.bin, $(PROGRAMS)))
ELFS=$(join $(PROGRAMS), $(patsubst %, /build/%.elf, $(PROGRAMS)))
DAS=$(join $(PROGRAMS), $(patsubst %, /build/%.da, $(PROGRAMS)))

TARGET=$(KERNEL_ELF)
KERNEL_ELF=$(filter %separation-kernel.elf, $(ELFS))
KERNEL_BIN=$(filter %separation-kernel.bin, $(BINS))
ROOT_BIN=$(filter %root.bin, $(BINS))
APP_BINS=$(filter-out $(KERNEL_BIN) $(ROOT_BIN), $(BINS))
S3K_H=separation-kernel/api/s3k.h

CLEAN=$(addsuffix .clean, $(PROGRAMS))

.PHONY: all clean api $(CLEAN) qemu $(KERNEL_ELF) $(ROOT_BIN) $(APPS_BINS) $(S3K_H)
.SECONDARY:


all: $(TARGET)

$(S3K_H):
	$(MAKE) -C separation-kernel api/s3k.h

$(APP_BINS): $(S3K_H)
	$(MAKE) -C $(basename $(notdir $@)) bin

$(ROOT_BIN): $(APP_BINS) api
	$(MAKE) -C $(basename $(notdir $@)) bin

$(KERNEL_ELF): $(ROOT_BIN) $(S3K_CONFIG_H)
	$(MAKE) -C $(basename $(notdir $@)) PAYLOAD=../$(ROOT_BIN) elf

clean: $(CLEAN)

$(CLEAN):
	$(MAKE) -C $(basename $@) clean

qemu: $(KERNEL_ELF)
	./scripts/qemu.sh $(KERNEL_ELF)
