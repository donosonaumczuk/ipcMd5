#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "include/slaveTest.h"
#include "./../src/include/slave.h"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int initSlaveSuite()
{
       return 0;
}




/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int cleanSlaveSuite()
{
       return 0;
}

void testOfReadingTheQuantityOfFilePathsToHash() {
  int fd, actualQuantity = 9, readQuantity;
  givenAFileDescriptorToReadAQuantity(&fd);
  //A quantity is valid if its quantity of digits is less or
  //equal to MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY
  givenAValidQuantityToReadOnAFile(fd, actualQuantity);

  readQuantity = whenTheNumberIsRead(fd);

  thenTheQuantityiesMustBeTheSame(actualQuantity, readQuantity);
}

void givenAFileDescriptorToReadAQuantity(int *fd) {
  int ret = open("testOfReadingTheQuantityOfFilePathsToHash.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if(ret == -1)
    perror("Couldn't open file.\n");
  *fd = ret;
}

void givenAValidQuantityToReadOnAFile(int fd, int actualQuantity) {
  char buffer[MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY + 1];
  sprintf(buffer, "%d", actualQuantity);
  if(write(fd, buffer, MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY) == -1)
    perror("Couldn't Write on file.\n");
}

int whenTheNumberIsRead(int fd) {
  lseek(fd, 0, SEEK_SET);
  return getNumberOfFilePaths(fd);  
}

void thenTheQuantityiesMustBeTheSame(int actualQuantity, int readQuantity) {
  CU_ASSERT(actualQuantity == readQuantity);
}

void testWriteHashOnAFIFO() {
  char *filePath, *fifoName;

  fifoName = givenAFifo();
  filePath = givenThePathOfAFileToHash();

  whenASlaveWritesAHashOnAFifo(fifoName, filePath);

  thenHashIsWrittenOnAFifo(fifoName);
}

char *givenAFifo() {
  if(mkfifo("fifoTest", S_IRUSR | S_IWUSR) == -1) {
    fprintf(stderr, "Error making a fifo.\n");
    exit(1);
  }
  return "fifoTest";
}

char *givenThePathOfAFileToHash() {
    return FILE_PATH_TO_HASH;
}

void whenASlaveWritesAHashOnAFifo(char *fifoName, char *filePathToHash) {
  int fifoFd = open(fifoName, O_RDWR);
  writeHashOnFd(fifoFd, filePathToHash);
}

void thenHashIsWrittenOnAFifo(char *fifoName) {
  int length = HASH_MD5_LENGTH + 2 + FILE_PATH_TO_HASH_LENGTH;
  char hash[length + 1];
  int fifoFd = open(fifoName, O_RDWR);
  read(fifoFd,hash, length);
  hash[length] = 0;
  CU_ASSERT(strcmp(hash, HASH) == 0);

}

void testReadAFilePath() {
   int fd;
   char *path;
   fd = givenAFileDescriptorWithSomethingWritten();

   path = whenAFilePathIsReadFromFileDescriptor(fd);

   thenFilesMustBeTheSame(path);
}

int givenAFileDescriptorWithSomethingWritten() {
   int fd = open("testReadAFilePath.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
   write(fd, FILE_PATH_TO_READ, FILE_PATH_TO_READ_LENGTH);
   lseek(fd, 0, SEEK_SET);
   return fd;
}


char *whenAFilePathIsReadFromFileDescriptor(int fd){
   char *path = getPath(fd);
   close(fd);
   return path;
}

void thenFilesMustBeTheSame(char *path) {

  CU_ASSERT(strcmp(path, "./test/PathName/ToRead.txt") == 0);

}

