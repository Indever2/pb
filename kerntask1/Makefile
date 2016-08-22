obj-m += test.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
.PHONY : clean
clean:
	rm -f modules.order Module.symvers test.ko test.mod.c test.mod.o test.o 
