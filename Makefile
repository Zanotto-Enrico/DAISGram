EXE = DAISGram
FLAGS = -std=c++11 -Wall -pedantic -lm
ADFLAG =
EXEPATH =

all: test_bmplib main main_tensor

debug: ADFLAG =-g
debug: EXEPATH =bin/debug
debug: main

release: ADFLAG =-O3
release: EXEPATH =bin/release
release: main


main: main.cpp tensor.o DAISGram.o libbmp.o 
	g++ $< $(EXEPATH)/tensor.o $(EXEPATH)/DAISGram.o $(EXEPATH)/libbmp.o $(FLAGS) $(ADFLAG) -o $(EXEPATH)/$(EXE)

tensor.o: tensor.cpp
	g++ $^ $(FLAGS) $(ADFLAG) -c -o $(EXEPATH)/$@

libbmp.o: libbmp.cpp
	g++ $^ $(FLAGS) $(ADFLAG) -c -o $(EXEPATH)/$@  

DAISGram.o: DAISGram.cpp libbmp.o
	g++ $< $(FLAGS) $(ADFLAG) -c -o $(EXEPATH)/$@  

test_bmplib: test_bmplib.cpp libbmp.o
	g++ $^ $(FLAGS) -o $< 

main_tensor: tensor.o main_tensor.cpp 
	g++ $^ $(FLAGS) -o $< 

cleandbg:
	rm bin/debug/*.o 
	rm bin/debug/$(EXE)

cleanrls:
	rm bin/release/*.o 
	rm bin/debug/$(EXE)