#ifndef SLAVE_TEST_H
#define SLAVE_TEST_H

#include <CUnit/Basic.h>
#include "./../../src/include/slave.h"


#define FILE_PATH_TO_READ "./test/PathName/ToRead.txt"
#define FILE_PATH_TO_HASH "fileToHash.txt"
#define FILE_QUANTITY 2
#define FILE_PATH_TO_READ_LENGTH 26
#define FILE_PATH_TO_HASH_LENGTH 14
#define HASH "fileToHash.txt: 9ea4d0ca436512ddc466ed8b549c4f8f"

int initSlaveSuite();
int cleanSlaveSuite();

void testReadAFilePath();
int givenAFileDescriptorWithSomethingWritten();
char *whenAFilePathIsReadFromFileDescriptor(int fd);
void thenFilesMustBeTheSame(char *path);

void testWriteHashOnAFIFO();
char *givenAFifo();
char *givenThePathOfAFileToHash();
void whenASlaveWritesAHashOnAFifo(char *fifoName, char *filePathToHash);
void thenHashIsWrittenOnAFifo(char *fifoName);

void testOfReadingTheQuantityOfFilePathsToHash();
void givenAFileDescriptorToReadAQuantity(int *fd);	
void givenAValidQuantityToReadOnAFile(int fd, int actualQuantity);
int whenTheNumberIsRead(int fd);
void thenTheQuantityiesMustBeTheSame(int actualQuantity, int readQuantity);

#endif