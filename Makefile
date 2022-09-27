
BUILD=build

# Prefix of toolchain
export RISCV_PREFIX?=riscv64-unknown-elf
# Kernel configuration file
export CONFIG_H=../config.h
export PAYLOAD=../build/root/root.bin

SUBDIRS=
SUBDIRS+=root separation-kernel

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ BUILD=../$(BUILD)/$@

clean:
	@echo "  CLEANING"
	@rm -rf $(BUILD)
