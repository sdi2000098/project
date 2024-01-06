CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O3

SRCS =  StoreTrainData.cpp ReadTrainData.cpp StoreQueryData.cpp ReadQueryData.cpp hFunc.cpp brute_force_reduced.cpp GetTrueDistances.cpp
SRCS2 =  StoreTrainData.cpp ReadTrainData.cpp StoreQueryData.cpp ReadQueryData.cpp hFunc.cpp brute_force.cpp

OBJS = $(SRCS:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)

EXEC = bruteReduced
EXEC2 = brute

bruteForceReduced: $(EXEC)
bruteForce: $(EXEC2)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC2): $(OBJS2)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean

