CC = gcc
CXX = g++
CXXFLAGS = -O3
CXXFLAGS += -I/home/.local/include
LDPATH += -L/home/.local/lib
TARGETS = boost_example boost_rf

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS)
