# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# Source files
SRCS =  StoreTrainData.cpp ReadTrainData.cpp main.cpp StoreQueryData.cpp ReadQueryData.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = main

# Default target
all: $(EXEC)

# Rule to build the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean target to remove object files and the executable
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean