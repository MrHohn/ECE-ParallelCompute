CC = mpic++

CFLAGS +=-c -g -O0 -Wall

LFLAGS = 

SOURCES = main.cpp GameOfLife.cpp GameWorker.cpp

OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LFLAGS) 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

run: $(EXECUTABLE)
	echo "*** now run sample input on 4 processes for 2 iterations ***"
	@sleep 1
	mpirun -np 4 -hostfile hostfile ./main 0 0 2
