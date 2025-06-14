CXX := g++
CC := gcc

CXXFLAGS := -std=c++20 -Wall -O2
CFLAGS := -std=c17 -Wall -O2

CXX_TARGET := parentCrawler
C_TARGET := childCrawler

CXX_SRC := crawler.cpp
C_SRC := crawler.c

all: $(CXX_TARGET) $(C_TARGET)

$(CXX_TARGET): $(CXX_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(C_TARGET): $(C_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean: 
	rm -f $(C_TARGET) $(CXX_TARGET)

