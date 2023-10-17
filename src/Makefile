CC = gcc
CXX = g++
CXXFLAGS = -O3
MaxCliqueOMP: CXXFLAGS += -fopenmp
TARGETS = MaxClique MaxCliquePivot MaxCliqueOMP MaxCliqueHybrid

.PHONY: all
all: $(TARGETS)

MaxCliqueHybrid: MaxCliqueHybrid.cc
	mpicxx $(CXXFLAGS) -fopenmp -o $@ $?

.PHONY: clean
clean:
	rm -f $(TARGETS)
