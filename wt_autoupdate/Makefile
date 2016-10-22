.PHONY: clean

OBJ = interpretator.o validator.o

build: $(OBJ) 
	gcc $(OBJ) -o check_exp

validator.o: validator.c header.h
	gcc validator.c -c

interpretator.o: interpretator.c header.h
	gcc interpretator.c -c

clean:
	rm -f *.o 
	rm -f check_exp