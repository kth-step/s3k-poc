BUILD=build
SRCS=$(wildcard *.[cS])
OBJS=$(patsubst %, $(BUILD)/%.o, $(SRCS))
LDS=root.lds

PREFIX?=riscv64-unknown-elf
CC=$(PREFIX)-gcc
OBJCOPY=$(PREFIX)-objcopy
OBJDUMP=$(PREFIX)-objdummp
SIZE=$(PREFIX)-size

CFLAGS=-T$(LDS) -nostartfiles
CFLAGS+=-O2

.PHONY: all
.SECONDARY:

all: $(OBJS)

$(BUILD)/%.c.o: %.c
	@mkdir -p $(@D)
	@echo -e "CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.S.o: %.S
	@mkdir -p $(@D)
	@echo -e "CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

%.elf: $(OBJS) $(LDS) 
	@mkdir -p $(@D)
	@echo -e "CC\t$@"
	@$(CC) $(CFLAGS) -o $@ $(OBJS)

%.bin: %.elf
	@mkdir -p $(@D)
	@echo -e "OBJCOPY\t$@"
	@$(OBJCOPY) -O binary $< $@

clean:
	@echo "Cleaning root"
	@rm -f $(OBJS)
