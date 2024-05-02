CXX = g++
CXXFLAGS = -Wall -std=c++20
TARGET = tail.out
SRC := Tail.cpp Usage.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET)