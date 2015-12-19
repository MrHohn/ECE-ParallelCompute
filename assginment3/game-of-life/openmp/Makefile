CC = g++

CFLAGS +=-c -g -O0 -Wall -fopenmp

LFLAGS = 

SOURCES = main.cpp GameOfLife.cpp

OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -fopenmp -o $@ $(LFLAGS) 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

run: $(EXECUTABLE)
	echo "*** now run sample input for 2 iterations ***"
	@sleep 1
	./main 0 0 2

run-large: $(EXECUTABLE)
	echo "*** now using random 2000*2000 board as input run for 100 iterations ***"
	@sleep 1
	./main 2000 2000 100
