CC = gcc
CXX = g++
CXXFLAGS = -O3 -fopenmp
TARGETS = time_pv time_omp time_hybrid

.PHONY: all
all: $(TARGETS)

time_hybrid: time_hybrid.cc
	mpicxx $(CXXFLAGS) -o $@ $?

.PHONY: clean
clean:
	rm -f $(TARGETS)
