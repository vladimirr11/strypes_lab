CXX = g++
TARGET = program
SRC := Program.cpp Swap.cpp

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET)

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET)