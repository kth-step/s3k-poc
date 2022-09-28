PROGRAM ?=root
BUILD   ?=build

TARGET ?=$(ELF) $(BIN)
ELF=$(BUILD)/$(PROGRAM).elf
BIN=$(BUILD)/$(PROGRAM).bin

LDS=root.lds

SRCS=$(wildcard src/*.[cS])
OBJS=$(patsubst %, $(BUILD)/%.o, $(SRCS))

RISCV_PREFIX?=riscv64-unknown-elf
CC=$(RISCV_PREFIX)-gcc
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdummp
SIZE=$(RISCV_PREFIX)-size

CFLAGS=-T$(LDS) -nostartfiles
CFLAGS+=-O2 -gdwarf-2
CFLAGS+=-fPIC -no-pie

.PHONY: all target clean
.SECONDARY:

all: target

target: $(TARGET)

$(BUILD)/%.c.o: %.c
	@mkdir -p $(@D)
	@printf "  CC\t$@\n"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.S.o: %.S
	@mkdir -p $(@D)
	@printf "  CC\t$@\n"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(ELF): $(OBJS) $(LDS) 
	@mkdir -p $(@D)
	@printf "  CC\t$@\n"
	@$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BIN): $(ELF) 
	@mkdir -p $(@D)
	@printf "  OBJCOPY\t$@\n"
	@$(OBJCOPY) -O binary $< $@

clean:
	@printf "  CLEANING\n"
	@rm -f $(TARGET) $(OBJS)
