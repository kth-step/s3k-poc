# S3K Proof-of-Concept Application

## How to build

Requires the riscv-gnu-toolchain (`riscv64-unknown-elf-*`)
```
make api [S3K_PATH=../s3k]
make all
```
By default, the makefile is configured so that the s3k repositort should
be a sibling, i.e., relative path to s3k should be `../s3k`.

## How to run with QEMU

Requires `qemu-system-riscv64`
```
make qemu [S3K_PATH=../s3k]
```
