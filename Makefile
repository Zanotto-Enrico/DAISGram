MAIN = main
FLAGS = -std=c++11 -lm
RLS =
DBG =

all: testbmp main main_tensor

debug: DBG=-g
debug: main

test: RLS=-O3
test: main

tensor.o: tensor.cpp
	g++ tensor.cpp -o tensor.o -c $(FLAGS)

libbmp.o: libbmp.cpp
	g++ libbmp.cpp -o libbmp.o -c $(FLAGS)

DAISGram.o: DAISGram.cpp libbmp.o
	g++ DAISGram.cpp -o DAISGram.o -c $(FLAGS)

main: libbmp.o tensor.o main.cpp 
	g++ $^ -o $(MAIN) $(FLAGS)

testbmp: test_bmplib.cpp libbmp.o
	g++ libbmp.o test_bmplib.cpp -o test_bmplib $(FLAGS)

main_tensor: tensor.o main_tensor.cpp 
	g++ tensor.o main_tensor.cpp -o main_tensor $(FLAGS)

clean:
	rm $(MAIN) *.o 