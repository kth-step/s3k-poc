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

function cleanup() {
        kill $(jobs -p)
}

trap cleanup EXIT

qemu-system-riscv64 -M virt -smp 5 -m 8G -nographic -bios none -kernel $KERNEL -s -S \
        -device loader,file=monitor/monitor.bin,addr=0x80010000 &

x-terminal-emulator -e                          \
riscv64-unknown-elf-gdb                         \
    -ex "set confirm off"                       \
    -ex "symbol-file $KERNEL"                   \
    -ex "add-symbol-file $PAYLOAD -o payload"   \
    -ex "j 0x80000000"                          \
    -ex "b hang"                                \
    -ex "b payload"                             \
    -ex "target remote localhost:1234"          \
    -ex "layout split"                          \
    -ex "fs cmd"
