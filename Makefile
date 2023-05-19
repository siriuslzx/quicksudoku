CXX = g++
LD = g++

Eigen3_DIR = /usr/include/eigen3
CXXFLAGS =  -Wall -O2 -std=c++11 -I$(Eigen3_DIR)

OBJ = example.o sudoku.o dfs.o

all: release

release: $(OBJ)
	$(LD) -o quickSudoku $(OBJ)

.PHONY : clean
clean :
	rm -f $(OBJ) quickSudoku
