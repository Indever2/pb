.PHONY : clean tester superviser itc clean_itc

all: superviser tester
	@echo "Building superviser & tester..."

superviser:
	gcc superviser.c -o sv

tester:
	gcc tester.c -o tester

itc:
	@echo "Making itc..."
	gcc itc.c -o itc

clean:
	@echo "Making all clean"
	rm -f tester
	rm -f sv

clean_itc:
	@echo "Making itc clean..."
	rm -f itc
