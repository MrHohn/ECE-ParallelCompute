# CC = gcc
CC = clang

CFLAGS +=-c -g -O3 -Wall

# LFLAGS = 

SOURCES = sqrtLocal.c

OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = sqrtLocal

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LFLAGS) 

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
