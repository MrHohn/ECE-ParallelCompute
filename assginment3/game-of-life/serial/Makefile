CC = g++

CFLAGS +=-c -g -O0 -Wall

LFLAGS = 

SOURCES = main.cpp GameOfLife.cpp

OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LFLAGS) 

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
