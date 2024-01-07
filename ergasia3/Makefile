CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O3

SRCS =  StoreTrainData.cpp ReadTrainData.cpp StoreQueryData.cpp ReadQueryData.cpp hFunc.cpp brute_force_reduced.cpp GetTrueDistances.cpp
SRCS2 =  StoreTrainData.cpp ReadTrainData.cpp StoreQueryData.cpp ReadQueryData.cpp hFunc.cpp brute_force.cpp
SRCS3 =  StoreTrainData.cpp ReadTrainData.cpp StoreQueryData.cpp ReadQueryData.cpp gnn.cpp lsh.cpp hFunc.cpp mrng.cpp graph_search_reduced.cpp GetTrueDistances.cpp
SRCS4 =  StoreTrainData.cpp ReadTrainData.cpp StoreQueryData.cpp ReadQueryData.cpp gnn.cpp lsh.cpp hFunc.cpp mrng.cpp graph_search.cpp
SRCS5 =  StoreTrainData.cpp ReadTrainData.cpp lshmain.cpp StoreQueryData.cpp ReadQueryData.cpp lsh.cpp hFunc.cpp
SRCS6 =  StoreTrainData.cpp ReadTrainData.cpp cube.cpp StoreQueryData.cpp ReadQueryData.cpp RandomProjection.cpp hFunc.cpp
SRCS7 =  StoreTrainData.cpp ReadTrainData.cpp hFunc.cpp  lsh.cpp RandomProjection.cpp kmeans.cpp KMeansMain.cpp 

OBJS = $(SRCS:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)
OBJS3 = $(SRCS3:.cpp=.o)
OBJS4 = $(SRCS4:.cpp=.o)
OBJS5 = $(SRCS5:.cpp=.o)
OBJS6 = $(SRCS6:.cpp=.o)
OBJS7 = $(SRCS7:.cpp=.o)

EXEC = bruteReduced
EXEC2 = brute
EXEC3 = graphReduced
EXEC4 = graph
EXEC5 = lsh
EXEC6 = cube
EXEC7 = cluster

all: bruteForceReduced bruteForce graphSearchReduced graphs lshh projection kmeans

bruteForceReduced: $(EXEC)
bruteForce: $(EXEC2)
graphSearchReduced : $(EXEC3)
graphs: $(EXEC4)
lshh: $(EXEC5)
projection: $(EXEC6)
kmeans: $(EXEC7)


$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC2): $(OBJS2)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC3): $(OBJS3)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC4): $(OBJS4)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC5): $(OBJS5)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC6): $(OBJS6)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC7): $(OBJS7)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean

