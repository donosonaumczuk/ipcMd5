CC	=	gcc
CFLAGS	=	-std=c99 -Wall -pedantic -I ./include

all:
	$(CC) $(CFLAGS)	applicationProcess.c errors.c tools.c -o hashMd5
	$(CC) $(CFLAGS)	slaveProcess.c slave.c tools.c errors.c -o slaveProcess
	$(CC) $(CFLAGS) viewProcess.c -o viewProcess

clean:
	rm hashMd5 slaveProcess viewProcess
