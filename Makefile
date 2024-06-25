CXX = g++
LD = g++

Eigen3_DIR = /usr/include/eigen3
CXXFLAGS =  -Wall -O2 -std=c++11 -I$(Eigen3_DIR)

OBJ = common.o dfs.o impl.o sudoku.o

all: release test

release: $(OBJ) example.o
	$(LD) -o quickSudoku $^

test: $(OBJ) test_speed.o
	$(LD) -o $@ $^

.PHONY : clean
clean :
	rm -f *.o quickSudoku test
