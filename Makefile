.POSIX:

# Prefix of toolchain
RISCV_PREFIX?=riscv64-unknown-elf

# Kernel configuration file
CONFIG_H=config.h

# Subdirectories
KERNEL=separation-kernel
TARGET=separation-kernel/build/separation-kernel.elf
PROGRAMS=root
PAYLOAD=root/build/root.bin

.PHONY: all clean $(KERNEL) $(PROGRAMS) qemu
.SECONDARY:

export RISCV_PREFIX

all: $(TARGET)

$(PROGRAMS):
	@printf "MAKE\t$@\n"
	@$(MAKE) -s -C $@

$(PAYLOAD): $(PROGRAMS)

$(KERNEL): $(CONFIG_H) $(PAYLOAD)
	@printf "MAKE\t$@\n"
	@$(MAKE) -s -C $@ CONFIG_H=../$(CONFIG_H) PAYLOAD=../$(PAYLOAD) 

$(TARGET): $(KERNEL)

clean:
	@for prog in $(PROGRAMS) $(KERNEL); do \
	    $(MAKE) -s -C $$prog clean; \
	done

qemu: $(TARGET)
	@printf "RUN QEMU\n"
	@./scripts/qemu.sh $(TARGET)
