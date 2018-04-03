CC	=	gcc
CFLAGS   = -std=c99 -Wall -pedantic -I ./include

all:
	$(CC) $(CFLAGS)	aplicationProcess.c errors.c tools.c -o aplicationProcess.out
	$(CC) $(CFLAGS)	slaveProcess.c slave.c tools.c errors.c -o slave

clean:
	rm *.out