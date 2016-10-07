.PHONY: all clean

CC = /home/ivan/prg/AltRep/rg54/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/bin/mips-linux-uclibc-gcc-4.4.7  -Wall -pedantic -o atput
all: main.c strtoint.c
	$(CC) main.c strtoint.c
clean: 
	rm -f atput