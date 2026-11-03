CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17

TARGET = matrix_lab
OBJS = matrix_lab.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix_lab.o: matrix_lab.cpp
	$(CXX) $(CXXFLAGS) -c $<

run: $(TARGET)
	./$(TARGET) input.txt

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean