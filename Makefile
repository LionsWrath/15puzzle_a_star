CC=g++
CONFIG=-std=c++11 -lm

default: a_estrela.cpp
	$(CC) $(CONFIG) a_estrela.cpp

clean: a.out
	rm a.out
