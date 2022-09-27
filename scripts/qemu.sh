#!/bin/env bash
PROGRAM=$1

if [ -z "$PROGRAM" ]; then
    echo "Arg 1, program/elf, missing."
    exit 1
fi

qemu-system-riscv64 -M virt -smp 5 -m 8G -nographic -bios none -kernel $PROGRAM -s -S &

x-terminal-emulator -e \
riscv64-unknown-elf-gdb \
    -ex "symbol-file $1" \
    -ex "target remote localhost:1234" \
    -ex "b hang" \
    -ex "layout split"

kill $(jobs -p)

