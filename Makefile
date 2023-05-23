CXX = g++
LD = g++

Eigen3_DIR = /usr/include/eigen3
CXXFLAGS =  -Wall -O2 -std=c++11 -I$(Eigen3_DIR)

OBJ = sudoku.o dfs.o

all: release test

release: example.o $(OBJ)
	$(LD) -o quickSudoku $^

test: test_speed.o $(OBJ)
	$(LD) -o $@ $^

.PHONY : clean
clean :
	rm -f *.o quickSudoku test
