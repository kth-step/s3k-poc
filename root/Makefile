PROGRAM ?=root
BUILD   ?=build

ELF=build/$(PROGRAM).elf
BIN=build/$(PROGRAM).bin
DA=build/$(PROGRAM).da

LDS=root.lds
API=../separation-kernel/api
PLATFORM=../separation-kernel/bsp/virt.h

OBJS=$(patsubst src/%.c, build/%.o, $(wildcard src/*.c)) \
     $(patsubst src/%.S, build/%.o, $(wildcard src/*.S))

RISCV_PREFIX?=riscv64-unknown-elf
CC=$(RISCV_PREFIX)-gcc
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdump
SIZE=$(RISCV_PREFIX)-size

CFLAGS+=-T$(LDS) -nostartfiles
CFLAGS+=-O2 -gdwarf-2
CFLAGS+=-static-pie -ffreestanding
CFLAGS+=-I$(API) -Iinc
CFLAGS+=-include $(PLATFORM)

PAYLOADS=../uart/build/uart.bin

.PHONY: all clean elf bin da
.SECONDARY:

all: elf bin da

elf: $(ELF)
bin: $(BIN)
da: $(DA)

target: $(TARGET)

build:
	mkdir -p $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: src/%.S $(PAYLOADS) | build
	$(CC) $(CFLAGS) -c -o $@ $<

$(ELF): $(OBJS) $(LDS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BIN): $(ELF) 
	$(OBJCOPY) -O binary $< $@

$(DA): $(ELF)
	$(OBJDUMP) -d $< > $@

clean:
	rm -f $(ELF) $(BIN) $(DA) $(OBJS)
