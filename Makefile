export PREFIX=riscv64-unknown-elf

SUBDIRS=separation-kernel root

export CONFIG_H=../s3k.h

.PHONY: $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ ../build/$@.elf ../build/$@.bin 

separation-kernel: root

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean $1; \
		rm -f build/$$dir.elf build/$$dir.bin; \
	done
