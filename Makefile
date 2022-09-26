
include config.mk

KERNEL=$(abspath s3k.elf)

.PHONY: $(KERNEL)

all: $(KERNEL)

$(KERNEL):
	$(MAKE) -C s3k $(KERNEL)
