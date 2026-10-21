KDIR ?= /usr/lib/modules/$(shell uname -r)/build
LLVM ?= 1

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) LLVM=$(LLVM) modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) LLVM=$(LLVM) clean

.PHONY: all clean
