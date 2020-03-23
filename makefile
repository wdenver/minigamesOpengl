CC=g++
CFLAGS=-c -Wall -O3
LDFLAGS=

INC=-I./
SOURCES = main.cpp

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=game


all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(INC) -lglut -lGLU -lGL -lm -w -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@


clean:
	rm -rf *o




