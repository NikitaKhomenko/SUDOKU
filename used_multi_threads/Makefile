TARGET = SUDOKU
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall
SRCPATH = ./source/
HEADPATH = ./header/

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = ./source/main.c ./source/mainFunc.c ./source/threadFunc.c
HEADERS = ./header/config.h ./header/mainFunc.h ./header/threadFunc.h

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall -pthread $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
