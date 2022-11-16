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


qemu-system-riscv64 -M virt -smp 5 -m 8G -nographic -bios none -kernel $KERNEL -s -S &

x-terminal-emulator -e                  \
riscv64-unknown-elf-gdb                 \
    -ex "symbol-file $KERNEL"           \
    -ex "target remote localhost:1234"  \
    -ex "b hang"                        \
    -ex "b exception_handler"           \
    -ex "layout split"                  \
    -ex "fs cmd"                        \
    -ex "b payload"

kill $(jobs -p)

