all: compile run clean

clean:
	rm fifoTest testReadAFilePath.txt

compile:
	gcc -Wall -pedantic -std=c99 mainTest.c slaveTest.c shmBuffTest.c ./../src/slave.c ./../src/tools.c ./../src/shmBuff.c -pthread -lrt -lcunit -o test

run:
	./test
