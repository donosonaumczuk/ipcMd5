all: compile run

clean:
	rm fifoTest testReadAFilePath.txt 

compile:
	gcc -Wall -pedantic -std=c99 slaveTest.c ./../src/slave.c -lcunit -o test

run: clean
	./test