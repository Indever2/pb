.PHONY : clean run.o graphics.o

HEADERS = include/game.h
OBJ = run.o graphics.o engine.o

all: $(OBJ) $(HEADERS)
	@echo -e "Building object files to DevLikeMaster"
	g++ $(OBJ) -o DevLikeMaster -lSDL2
	@echo -e "   ...done\n"

run.o: run.cpp $(HEADERS)
	@echo -e "Building run.cpp -> run.o"
	g++ run.cpp -c
	@echo -e "   ...done\n"

graphics.o: src/graphics.cpp $(HEADERS)
	@echo -e "Building graphics.cpp -> graphics.o"
	g++ src/graphics.cpp -c -lSDL2
	@echo -e "   ...done\n"

engine.o: src/engine.cpp $(HEADERS)
	@echo -e "Building engine.cpp -> engine.o"
	g++ src/engine.cpp -c
	@echo -e "   ...done\n"

clean:
	@echo -e "Cleaning in the root dir..."
	rm -f *.o
	rm -f DevLikeMaster
	@echo -e "   ...done\n"
	cd include/ && make clean
	@echo -e "\n....Clean up - finish succesful....\n"
