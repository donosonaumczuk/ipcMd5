#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "include/slaveTest.h"
#include "./../src/include/slave.h"


//#define HASH ""

int main()
{
   CU_pSuite slaveSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   slaveSuite = CU_add_suite("slaveSuite", initSlaveSuite, cleanSlaveSuite);
   if (NULL == slaveSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   // /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   // if((CU_add_test(slaveSuite, "test of read a file path()", testReadAFilePath) == NULL) ||
   //  (CU_add_test(slaveSuite, "test of write the hash of a file on a file()", testWriteAHashOnAFile) == NULL)) {
   //    CU_cleanup_registry();
   //    return CU_get_error();
   //  }
   if((CU_add_test(slaveSuite, "test of Writting a Hash on a fifo.\n", testWriteHashOnAFIFO) == NULL)
    || (CU_add_test(slaveSuite, "test of read a file path()", testReadAFilePath) == NULL)) {
      CU_cleanup_registry();
      return CU_get_error();
    }
   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}

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
  read(fifoFd,hash,length);
  hash[length] = 0;
  CU_ASSERT(strcmp(hash,HASH) == 0);

}

void testWriteAHashOnAFile(){
   int emptyFileFd;
   givenAFileDescriptorThatIsEmpty(&emptyFileFd);
   givenAPathOfAFile();

   whenHashIsWrittenOnAFile(emptyFileFd,FILE_PATH_TO_HASH);

   thenFileIsWrittenCorrectly(emptyFileFd);
}

void givenAFileDescriptorThatIsEmpty(int *fd) {
   *fd = open("testWriteAHashOnAFile.txt", O_CREAT | O_RDWR);
   if(*fd == -1) {
      fprintf(stderr, "Error opening file.\n");
   }
}

void givenAPathOfAFile() {
   int fd = open(FILE_PATH_TO_HASH,O_CREAT|O_RDWR);
   write(fd,"Text on the file to be hashed.",strlen("Text on the file to be hashed.")+1);
   close(fd);
}

void whenHashIsWrittenOnAFile(int fd, char *filePath) {
   writeHashOnFd(fd,filePath);
}

void thenFileIsWrittenCorrectly(int emptyFileFd) {
     CU_ASSERT(10 == 10);

}


void testReadAFilePath() {
   int fd;
   char * path;
   fd = givenAFileDescriptorWithSomethingWritten();

   path = whenAFilePathIsReadFromFileDescriptor(fd);
   
   thenFilesMustBeTheSame(path);
}

int givenAFileDescriptorWithSomethingWritten() {
   int fd = open("testReadAFilePath.txt", O_CREAT|O_RDWR,777);
   write(fd,FILE_PATH_TO_READ,FILE_PATH_TO_READ_LENGTH);
   lseek(fd,0,SEEK_SET);
   return fd;
}


char *whenAFilePathIsReadFromFileDescriptor(int fd){
   char * path = getPath(fd);
   close(fd);
   return path;
}

void thenFilesMustBeTheSame(char * path) {
   
  CU_ASSERT(strcmp(path, "./test/PathName/ToRead.txt") == 0);  

}


