obj-m += task15.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
.PHONY : clean
clean:
	rm -f modules.order Module.symvers task15.ko task15.mod.c task15.mod.o task15.o 
