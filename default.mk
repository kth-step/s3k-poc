ELF=$(BUILD)/$(PROGRAM).elf
BIN=$(BUILD)/$(PROGRAM).bin
DA =$(BUILD)/$(PROGRAM).da

BUILD=build
SRC=src
COMMON=../common
INC=inc

LDS ?=../default.lds
API=../separation-kernel/api
BSP=../separation-kernel/bsp/virt

OBJS=$(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(wildcard $(SRC)/*.c)) \
     $(patsubst $(SRC)/%.S, $(BUILD)/%.o, $(wildcard $(SRC)/*.S)) \
     $(patsubst $(COMMON)/%.c, $(BUILD)/%.o, $(wildcard $(COMMON)/*.c))
DAS=$(OBJS:.o=.da)

RISCV_PREFIX?=riscv64-unknown-elf
CC=$(RISCV_PREFIX)-gcc
LD=$(RISCV_PREFIX)-ld
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdump
SIZE=$(RISCV_PREFIX)-size

ARCH=rv64imac
ABI=lp64
CMODEL=medany

CFLAGS+=-march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CMODEL)
CFLAGS+=-std=gnu18
CFLAGS+=-Wall
CFLAGS+=-O0 -gdwarf-2
CFLAGS+=-I$(API) -I$(INC) -I$(BSP) -I../common

CFLAGS+=-T$(LDS) -nostartfiles -static-pie
CFLAGS+=-ffreestanding -mno-relax

.PHONY: all clean elf bin da
.SECONDARY:

all: elf bin da

elf: $(ELF)
bin: $(BIN)
da: $(DA) $(DAS)

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(SRC)/%.S | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(COMMON)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(COMMON)/%.S | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(ELF): $(OBJS) $(LDS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BIN): $(ELF) 
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.da: $(BUILD)/%.o
	$(OBJDUMP) -d $< > $@

$(DA): $(ELF)
	$(OBJDUMP) -d $< > $@

clean:
	rm -f $(ELF) $(BIN) $(DA) $(OBJS)

format:
	clang-format -i $(wildcard src/*.[hc])

size: $(OBJS) $(ELF)
	$(SIZE) $(OBJS) $(ELF)
