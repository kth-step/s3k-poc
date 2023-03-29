#!/usr/bin/env bash
KERNEL=$1
PAYLOAD=$2

if [ -z "$KERNEL" ]; then
	echo "Arg 1, KERNEL elf, missing."
	exit 1
fi

if [ -z "$PAYLOAD" ]; then
	echo "Arg 2, PAYLOAD elf, missing."
	exit 1
fi

PAYLOAD_BIN=${PAYLOAD%.elf}.bin

riscv64-unknown-elf-objcopy -O binary $PAYLOAD $PAYLOAD_BIN

st -e   qemu-system-riscv64 -M virt -smp 1 -m 128M			\
	-nographic -bios none -kernel $KERNEL					\
	-device loader,file=$PAYLOAD_BIN,addr=0x80010000			\
	-s -S									&

#        -serial tcp:localhost:4321,server,nowait &

sleep 1

st -e	riscv64-unknown-elf-gdb							\
	-ex "set confirm off"							\
	-ex "set pagination off"						\
	-ex "set output-radix 16"						\
	-ex "symbol-file $KERNEL"						\
	-ex "add-symbol-file $PAYLOAD 0x80010000"				\
	-ex "j 0x80000000"							\
	-ex "b _hang"								\
	-ex "b *0x80010000"							\
	-ex "b handle_exception if mepc == 0"					\
	-ex "target remote localhost:1234"					\
	-ex "layout split"							\
	-ex "fs cmd"

pkill -P $$
