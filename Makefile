
SUBDIRS=separation-kernel root
BUILD=build

export PREFIX=riscv64-unknown-elf
export CONFIG_H=../s3k.h

.PHONY: $(SUBDIRS) $(CLEAN_DIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ BUILD=../$(BUILD)/$@

separation-kernel: root

clean:
	@echo "Cleaning"
	@rm -rf $(BUILD)
