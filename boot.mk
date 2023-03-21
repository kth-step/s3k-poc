boot.elf: boot/main.o boot/capman.o boot/payload.o common/s3k-syscall.o common/s3k-utils.o common/start.o
	$(LD) $(LDFLAGS) -o $@ $^
