obj-m += task16.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
.PHONY : clean
clean:
	rm -f modules.order Module.symvers task16.ko task16.mod.c task16.mod.o task16.o 
