CXX = clang++-14
CXXFLAGS = -std=c++14 -Wall -pedantic -pthread

all: st_pipeline

st_pipeline: st_pipeline.o active_object.o safe_queue.o prime.o
	$(CXX) $(CXXFLAGS) -o st_pipeline st_pipeline.o active_object.o safe_queue.o prime.o

st_pipeline.o: st_pipeline.cpp active_object.hpp prime.cpp safe_queue.hpp
	$(CXX) $(CXXFLAGS) -c st_pipeline.cpp

active_object.o: active_object.cpp active_object.hpp safe_queue.hpp
	$(CXX) $(CXXFLAGS) -c active_object.cpp

safe_queue.o: safe_queue.cpp safe_queue.hpp
	$(CXX) $(CXXFLAGS) -c safe_queue.cpp

prime.o: prime.cpp
	$(CXX) $(CXXFLAGS) -c prime.cpp

clean:
	rm -f st_pipeline *.o