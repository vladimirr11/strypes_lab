CXX = g++
CXXFLAGS = -Wall -std=c++20
TARGET = bitwise_op
SRC := BitwiseOp.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET)