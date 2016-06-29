all: 
	gcc task1_2.c -o server
	gcc task1_2cl.c -o client
	./server
clean:
	rm -f server
	rm -f client
