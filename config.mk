# See LICENSE file for copyright and license details.

# Prefix of GNU toolchain
RISCV_PREFIX=riscv64-unknown-elf-

# Set tool variables
export AR=$(RISCV_PREFIX)ar
export AS=$(RISCV_PREFIX)as
export CC=$(RISCV_PREFIX)gcc
export CXX=$(RISCV_PREFIX)g++
export CPP=$(RISCV_PREFIX)cpp
export ELFEDIT=$(RISCV_PREFIX)elfedit
export COV=$(RISCV_PREFIX)gcov
export GDB=$(RISCV_PREFIX)gdb
export PROF=$(RISCV_PREFIX)gprof
export LD=$(RISCV_PREFIX)ld
export NM=$(RISCV_PREFIX)nm
export OBJCOPY=$(RISCV_PREFIX)objcopy
export OBJDUMP=$(RISCV_PREFIX)objdump
export READELF=$(RISCV_PREFIX)readelf
export SIZE=$(RISCV_PREFIX)size
export STRING=$(RISCV_PREFIX)string
export STRIP=$(RISCV_PREFIX)strip
