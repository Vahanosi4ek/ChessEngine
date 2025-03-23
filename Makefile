CPP:=$(wildcard *.cpp */*.cpp *.cc */*.cc)

all: run clean

run: build
	./main

build:
	g++ -O3 -Wall -Wextra -std=c++20 $(CPP) -o main

clean:
	rm -f main