CC	=	gcc
CFLAGS	=	-std=c99 -Wall -pedantic -I ./include
SEMFLAGS =  -lpthread -lrt

all:
	$(CC) $(CFLAGS)	applicationProcess.c errors.c tools.c shmBuff.c -o hashMd5 $(SEMFLAGS)
	$(CC) $(CFLAGS)	slaveProcess.c slave.c tools.c errors.c -o slaveProcess $(SEMFLAGS)
	$(CC) $(CFLAGS) viewProcess.c shmBuff.c errors.c tools.c -o viewProcess $(SEMFLAGS)
clean:
	rm hashMd5 slaveProcess viewProcess hashMd5LastResult.txt availableSlavesQueue md5ResultQueue
