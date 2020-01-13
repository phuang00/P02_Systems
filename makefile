ifeq ($(DEBUG), true)
	CC = gcc -g
else
	CC = gcc
endif

all: setup.o battleship.o board_fxns.o
	$(CC) -o setup setup.o board_fxns.o
	$(CC) -o play battleship.o board_fxns.o

setup.o: setup.c battleship.h
	$(CC) -c setup.c

battleship.o: battleship.c battleship.h
	$(CC) -c battleship.c

board_fxns.o: board_fxns.c battleship.h
	$(CC) -c board_fxns.c

clean:
	rm *.o
	rm *~
