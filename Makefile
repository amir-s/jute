CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic

.PHONY: all clean test

all: jute

jute: main.cpp jute.cpp jute.h
	$(CXX) $(CXXFLAGS) -o $@ main.cpp jute.cpp

test: test_jute
	./test_jute

test_jute: test_jute.cpp jute.cpp jute.h
	$(CXX) $(CXXFLAGS) -o $@ test_jute.cpp jute.cpp

clean:
	rm -f jute test_jute
