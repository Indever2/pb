obj-m += task13.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
.PHONY : clean
clean:
	rm -f modules.order Module.symvers task13.ko task13.mod.c task13.mod.o task13.o 
