CC	=	gcc
C_POST_FLAGS =  -lpthread -lrt -lcunit
SRC = ../src
CFLAGS	=	-std=c99 -Wall -pedantic -I $(SRC)/include -I ./include


all:
	$(CC) $(CFLAGS) mainTest.c shmBuffTest.c slaveTest.c $(SRC)/errors.c $(SRC)/slaveProcess.c $(SRC)/slave.c $(SRC)/applicationProcess.c $(SRC)/shmBuff.c $(SRC)/tools.c -o test $(C_POST_FLAGS)

clean:
	rm test;
	cd $(SRC);
	make clean
