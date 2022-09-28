.POSIX:

# Prefix of toolchain
RISCV_PREFIX?=riscv64-unknown-elf

# Kernel configuration file
CONFIG_H=../config.h

# Build directory
BUILD?=build

# Subdirectories
KERNEL=separation-kernel
PROGRAMS=root
PAYLOAD=../build/root/root.bin

.PHONY: all clean $(KERNEL) $(PROGRAMS)
.SECONDARY:

export RISCV_PREFIX

all: $(KERNEL) $(PROGRAMS)


$(PAYLOAD):
	@for prog in $(PROGRAMS); do \
	    $(MAKE) -C $$prog BUILD=../$(BUILD)/$$prog; \
	done
$(CONFIG_H): $(PAYLOAD)
	@touch $(CONFIG_H)
$(KERNEL): $(CONFIG_H) $(PAYLOAD)
	@$(MAKE) -C $@ BUILD=../$(BUILD)/$@ CONFIG_H=$(CONFIG_H)

clean:
	@printf "  CLEANING\n"
	@rm -rf $(BUILD)

