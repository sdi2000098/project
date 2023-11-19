# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# Add the -fopenmp flag for OpenMP support
CXXFLAGS += -fopenmp

# Source files
SRCS =  StoreTrainData.cpp ReadTrainData.cpp lshmain.cpp StoreQueryData.cpp ReadQueryData.cpp lsh.cpp hFunc.cpp
SRCS2 =  StoreTrainData.cpp ReadTrainData.cpp cube.cpp StoreQueryData.cpp ReadQueryData.cpp RandomProjection.cpp hFunc.cpp
SRCS3 =  StoreTrainData.cpp ReadTrainData.cpp hFunc.cpp  lsh.cpp RandomProjection.cpp kmeans.cpp KMeansMain.cpp 
SRCS4 =  StoreTrainData.cpp ReadTrainData.cpp gnnmain.cpp StoreQueryData.cpp ReadQueryData.cpp lsh.cpp hFunc.cpp 

# Object files
OBJS = $(SRCS:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)
OBJS3 = $(SRCS3:.cpp=.o)
OBJS4 = $(SRCS4:.cpp=.o)
# Executable name
EXEC = lsh
EXEC2 = cube
EXEC3 = cluster
EXEC4 = gnn

# Default target
lshh: $(EXEC)
projection: $(EXEC2)
kmeans: $(EXEC3)
gnns: $(EXEC4)

# Rule to build the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC2): $(OBJS2)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC3): $(OBJS3)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC4): $(OBJS4)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean target to remove object files and the executable
clean:
	rm -f $(OBJS) $(EXEC) $(OBJS2) $(EXEC2) $(OBJS3) $(EXEC3) $(EXEC4) $(OBJS4) 

.PHONY: all clean
