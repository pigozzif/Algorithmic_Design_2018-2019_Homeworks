CXX = c++
CXXFLAGS = -std=c++11 -Wall -Wextra -I .
TARGET = rbt.x
SRC = RedBlack.cc

all: $(TARGET)

$(TARGET): $(SRC)
	    $(CXX) $(CXXFLAGS) $^ -o $@

$(SRC): ./BST.h

clean:
	rm $(TARGET)

.PHONY: all clean
