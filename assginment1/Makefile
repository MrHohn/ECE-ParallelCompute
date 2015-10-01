CC = gcc

CFLAGS +=-c -g -O0 -Wall -std=c++11

# LFLAGS = 

SOURCES = sqrtLocal.c

OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = sqrtLocal

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LFLAGS) 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)