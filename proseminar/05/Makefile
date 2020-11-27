flags=-g

all: nbody

nbody: nbody.cpp particle.cpp
	g++ $(flags) $^ -o $@

.PHONY: clean
clean:
	rm -f nbody stars.dat