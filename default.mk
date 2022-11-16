ELF=$(BUILD)/$(PROGRAM).elf
BIN=$(BUILD)/$(PROGRAM).bin
DA =$(BUILD)/$(PROGRAM).da

BUILD=build
SRC=src
INC=inc

LDS=../default.lds
API=../separation-kernel/api
PLATFORM=../bsp/virt.h

OBJS=$(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(wildcard $(SRC)/*.c)) \
     $(patsubst $(SRC)/%.S, $(BUILD)/%.o, $(wildcard $(SRC)/*.S))

RISCV_PREFIX?=riscv64-unknown-elf
CC=$(RISCV_PREFIX)-gcc
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdump
SIZE=$(RISCV_PREFIX)-size

ARCH   =rv64imac
ABI    =lp64
CMODEL =medlow

CFLAGS+=-march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CMODEL)
CFLAGS+=-std=gnu18
CFLAGS+=-Wall -fanalyzer -Werror
CFLAGS+=-O2 -gdwarf-2
CFLAGS+=-I$(API) -I$(INC) -include $(PLATFORM)

CFLAGS+=-T$(LDS) -nostartfiles -mno-relax
CFLAGS+=-static -fpie -ffreestanding -Wl,--no-dynamic-linker

.PHONY: all clean elf bin da
.SECONDARY:

all: elf bin da

elf: $(ELF)
bin: $(BIN)
da: $(DA)

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(SRC)/%.S | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(ELF): $(OBJS) $(LDS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BIN): $(ELF) 
	$(OBJCOPY) -O binary $< $@

$(DA): $(ELF)
	$(OBJDUMP) -d $< > $@

clean:
	rm -f $(ELF) $(BIN) $(DA) $(OBJS)
