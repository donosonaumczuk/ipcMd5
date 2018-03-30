all: clean compile

clean:
	rm fifoTest

compile:
	gcc -Wall -pedantic -std=c99 slaveTest.c ./../src/slave.c -lcunit -o test

run:
	./test