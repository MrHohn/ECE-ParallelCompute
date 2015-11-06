EXECUTABLE := PrefixSum

all: $(EXECUTABLE)

###########################################################

OBJDIR=objs
CXX=g++
CXXFLAGS=-O0 -Wall -m64 -std=c++11
LDFLAGS=-L/usr/local/cuda/lib64/ -lcudart
NVCC=nvcc
NVCCFLAGS=-O0 -arch=sm_30 -m64

OBJS=$(OBJDIR)/main.o  $(OBJDIR)/PrefixSum_serial.o  $(OBJDIR)/util.o  $(OBJDIR)/PrefixSum_parallel.o

.PHONY: dirs clean

default: $(EXECUTABLE)

dirs:
		mkdir -p $(OBJDIR)/

clean:
		rm -rf $(OBJDIR) *.ppm *~ $(EXECUTABLE) $(LOGS)

$(EXECUTABLE): dirs $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/%.o: %.cu
		$(NVCC) $< $(NVCCFLAGS) -c -o $@
