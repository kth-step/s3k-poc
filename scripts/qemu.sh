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

TMPBIN=$(mktemp /tmp/XXXXX.bin)

riscv64-unknown-elf-objcopy -O binary $PAYLOAD $TMPBIN

function cleanup() {
        rm -f $TMPBIN
        rm -f $TMPFIFO
        kill $(jobs -p)
}

trap cleanup EXIT

qemu-system-riscv64 -M virt -smp 1 -m 8G -nographic -bios none -kernel $KERNEL \
        -device loader,file=$TMPBIN,addr=0x80010000 -s -S &
#        -serial tcp:localhost:4321,server,nowait &

st -e                                                   \
riscv64-unknown-elf-gdb                                 \
        -ex "set confirm off"                           \
        -ex "set pagination off"                        \
        -ex "symbol-file $KERNEL"                       \
        -ex "add-symbol-file $PAYLOAD"    \
        -ex "j 0x80000000"                              \
        -ex "b _hang"                                   \
        -ex "b *0x80010000"                             \
	-ex "b handle_exception"                        \
        -ex "target remote localhost:1234"              \
        -ex "layout split"                              \
        -ex "fs cmd"
