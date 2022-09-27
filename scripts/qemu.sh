#!/bin/env bash
PROGRAM=$1

qemu-system-riscv64 -smp 5 -nographic -bios none -kernel $PROGRAM 

