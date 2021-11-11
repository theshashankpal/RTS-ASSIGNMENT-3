CC = gcc
OBJ = scheduler.o
CFLAGS = -c
LFLAGS = -lpthread -lm

vpath %.c src
vpath %.h src

output: $(OBJ) 
	$(CC) scheduler.o -o $@ $(LFLAGS)


scheduler.o: scheduler.c project.h
	$(CC) $(CFLAGS) $^ 

clean: 
	rm -rf *.o output