flags=-O2 -ffast-math -std=c++11
# flags=-g -std=c++11

all: nbody

nbody: nbody.cpp particle.cpp constants.cpp
	g++ $(flags) $^ -lm -o $@

.PHONY: clean
clean:
	rm -f nbody stars.dat *.log
