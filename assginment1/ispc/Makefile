
CXX=clang++ -m32
CXXFLAGS=-Iobjs/ -O3 -Wall
ISPC=ispc
ISPCFLAGS=-O2 --arch=x86 --target=sse2

default: sqrt

.PHONY: dirs clean
.PRECIOUS: objs/sqrt.h

dirs:
	/bin/mkdir -p objs/

clean:
	/bin/rm -rf objs *~ sqrt

sqrt: dirs  objs/sqrt.o objs/sqrt_ispc.o objs/sqrt_serial.o
	$(CXX) $(CXXFLAGS) -o $@ objs/sqrt.o objs/sqrt_ispc.o objs/sqrt_serial.o

objs/sqrt.o: sqrt.cpp objs/sqrt_ispc.h 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

objs/sqrt_serial.o: sqrt_serial.cpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

objs/%_ispc.h objs/%_ispc.o: %.ispc
	$(ISPC) $(ISPCFLAGS) $< -o objs/$*_ispc.o -h objs/$*_ispc.h
