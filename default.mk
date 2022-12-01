ELF=$(PROGRAM).elf
BIN=$(PROGRAM).bin
DA =$(PROGRAM).da

SRC=src
INC=inc
COMMON=../common
API=../separation-kernel/api

LDS ?=../default.lds
BSP=../separation-kernel/bsp/virt

SRCS=$(wildcard $(SRC)/*.[cS]) \
     $(wildcard $(COMMON)/*.[cS]) \
     $(wildcard $(API)/*.[cS])
HDRS=$(wildcard $(SRC)/*.h) \
     $(wildcard $(COMMON)/*.h) \
     $(wildcard $(API)/*.h)


RISCV_PREFIX?=riscv64-unknown-elf
CC=$(RISCV_PREFIX)-gcc
LD=$(RISCV_PREFIX)-ld
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdump
SIZE=$(RISCV_PREFIX)-size

CFLAGS+=-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-std=c18
CFLAGS+=-Wall
CFLAGS+=-O3 -gdwarf-2
CFLAGS+=-I$(API) -I$(INC) -I$(BSP) -I../common
CFLAGS+=-T $(LDS) -nostartfiles -ffreestanding -mno-relax -static-pie

.PHONY: all clean elf bin da
.SECONDARY:

all: elf bin da

elf: $(ELF)
bin: $(BIN)
da: $(DA) $(DAS)

$(ELF): $(SRCS) $(HDRS) $(LDS) 
	$(CC) $(CFLAGS) -o $@ $(SRCS)

$(BIN): $(ELF) 
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.da: $(BUILD)/%.o
	$(OBJDUMP) -d $< > $@

$(DA): $(ELF)
	$(OBJDUMP) -d $< > $@

clean:
	rm -f $(ELF) $(BIN) $(DA)

format:
	clang-format -i $(wildcard src/*.[hc])

size: $(OBJS) $(ELF)
	$(SIZE) $(OBJS) $(ELF)
