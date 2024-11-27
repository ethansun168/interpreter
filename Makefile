# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Files
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
TARGET = main
TEST_DIR = tests

INPUT_FILE = input.txt
EXPECTED_OUTPUT = expected_output.txt
ACTUAL_OUTPUT = actual_output.txt

# Default target
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compile step
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET) < $(INPUT_FILE) > $(ACTUAL_OUTPUT)
	@diff -q $(ACTUAL_OUTPUT) $(EXPECTED_OUTPUT) || (echo "Test failed: output does not match expected" && exit 1)
	@echo "Test successful!"

# Clean target
clean:
	rm -f $(OBJS) $(TARGET) $(ACTUAL_OUTPUT)
