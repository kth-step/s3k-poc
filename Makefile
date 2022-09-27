
SUBDIRS=separation-kernel root
CLEAN_DIRS=$(addsuffix .clean, $(SUBDIRS))

export PREFIX=riscv64-unknown-elf
export CONFIG_H=../s3k.h

.PHONY: $(SUBDIRS) $(CLEAN_DIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ ../build/$@.elf ../build/$@.bin 

separation-kernel: root

clean: $(CLEAN_DIRS)
	@rm -rf build

$(CLEAN_DIRS):
	@$(MAKE) -s -C $(basename $@) clean
