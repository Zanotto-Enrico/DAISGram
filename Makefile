FLAGS = -std=c++11 -pedantic -Wall -lm
EXE = 
DBG =
RLS =

all: testbmp main main_tensor

debug: DBG= -g 
debug: EXE = debug
debug: main

test: RLS= -O3 
test: EXE = test
test: main

tensor.o: tensor.cpp
	g++ tensor.cpp -c $(FLAGS) $(RLS) $(DBG) -o tensor.o 

libbmp.o: libbmp.cpp
	g++ libbmp.cpp -c $(FLAGS) $(RLS) $(DBG) -o libbmp.o 

DAISGram.o: DAISGram.cpp libbmp.o
	g++ DAISGram.cpp -c $(FLAGS) $(RLS) $(DBG) -o DAISGram.o 

main: libbmp.o tensor.o main.cpp 
	g++ $^ $(FLAGS) $(RLS) $(DBG) -o $(EXE) 

testbmp: test_bmplib.cpp libbmp.o
	g++ libbmp.o test_bmplib.cpp -o test_bmplib $(FLAGS)

main_tensor: tensor.o main_tensor.cpp 
	g++ tensor.o main_tensor.cpp -o main_tensor $(FLAGS)

clean:
	rm $(EXE) *.o 