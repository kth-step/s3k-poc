PROGRAM ?=uart
BUILD   ?=build

TARGET ?=$(ELF) $(BIN)
ELF=$(BUILD)/$(PROGRAM).elf
BIN=$(BUILD)/$(PROGRAM).bin

LDS=uart.lds

OBJS=$(patsubst %.c, $(BUILD)/%.o, $(wildcard src/*.c)) \
     $(patsubst %.S, $(BUILD)/%.o, $(wildcard src/*.S))

RISCV_PREFIX?=riscv64-unknown-elf
CC=$(RISCV_PREFIX)-gcc
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdump
SIZE=$(RISCV_PREFIX)-size

CFLAGS=-T$(LDS) -nostartfiles
CFLAGS+=-O2 -gdwarf-2
CFLAGS+=-fPIC -no-pie

.PHONY: all target clean
.SECONDARY:

all: target

target: $(TARGET)

$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	@printf "CC\t$@\n"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.S
	@mkdir -p $(@D)
	@printf "CC\t$@\n"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(ELF): $(OBJS) $(LDS) 
	@mkdir -p $(@D)
	@printf "CC\t$@\n"
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BIN): $(ELF) 
	@mkdir -p $(@D)
	@printf "OBJCOPY\t$@\n"
	@$(OBJCOPY) -O binary $< $@

clean:
	@printf "CLEAN\t$(PROGRAM)\n"
	@rm -f $(TARGET) $(OBJS)
