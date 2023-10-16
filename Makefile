# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# Source files
SRCS =  StoreTrainData.cpp ReadTrainData.cpp main.cpp StoreQueryData.cpp ReadQueryData.cpp lsh.cpp hFunc.cpp
SRCS2 =  StoreTrainData.cpp ReadTrainData.cpp cube.cpp StoreQueryData.cpp ReadQueryData.cpp RandomProjection.cpp hFunc.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)
# Executable name
EXEC = main
EXEC2 = cube

# Default target
all: $(EXEC)
projection: $(EXEC2)

# Rule to build the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC2): $(OBJS2)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean target to remove object files and the executable
clean:
	rm -f $(OBJS) $(EXEC) $(OBJS2) $(EXEC2) 

.PHONY: all clean