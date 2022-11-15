PROGRAM ?=monitor
BUILD   ?=build

ELF=$(BUILD)/$(PROGRAM).elf
BIN=$(BUILD)/$(PROGRAM).bin
DA=$(BUILD)/$(PROGRAM).da

LDS=default.lds

OBJS=$(patsubst %.c, $(BUILD)/%.o, $(wildcard src/*.c)) \
     $(patsubst %.S, $(BUILD)/%.o, $(wildcard src/*.S))

RISCV_PREFIX?=riscv64-unknown-elf
CC=$(RISCV_PREFIX)-gcc
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdump
SIZE=$(RISCV_PREFIX)-size

CFLAGS+=-T$(LDS) -nostartfiles
CFLAGS+=-O2 -gdwarf-2
CFLAGS+=-static-pie -ffreestanding

.PHONY: all target clean elf bin da
.SECONDARY:

all: elf bin da

elf: $(ELF)
bin: $(BIN)
da: $(DA)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.S | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(ELF): $(OBJS) $(LDS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BIN): $(ELF) 
	@printf "  OBJCOPY\t$@\n"
	@$(OBJCOPY) -O binary $< $@

$(DA): $(ELF)
	$(OBJDUMP) -d $< > $@

clean:
	rm -f $(ELF) $(BIN) $(DA) $(OBJS)
