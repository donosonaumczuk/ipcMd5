#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//defines from slave
#define BLOCK 10
#define TRUE     1
#define FALSE    0
//defines from test
#define FILE_PATH_TO_READ "./test/PathName/ToRead.txt"
#define FILE_PATH_TO_HASH "FileToHash.txt"
#define FILE_QUANTITY 2
#define FILE_PATH_LENGTH 26
//#define HASH ""

//functions from slave.c
unsigned char * getPath(int fd);
int initSlaveSuite();
//functions for this test.c
void testReadAFilePath();
int cleanSlaveSuite();
int initSlaveSuite();
void givenAFileDescriptor();
char * WhenAFilePathIsReadFromFileDescriptor(int fd);
/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int initSlaveSuite()
{
   // if (NULL == (temp_file = fopen("temp.txt", "w+"))) {
   //    return -1;
   // }
   // else {
       return 0;
   // }
}




/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int cleanSlaveSuite()
{
   // if (0 != fclose(temp_file)) {
   //    return -1;
   // }
   // else {
   //    temp_file = NULL;
   //    return 0;
   // }
}


// void testWriteAHashOnAFile(){
//    int emptyFileFd;
//    emptyFileFd = givenAFileDescriptorThatIsEmpty();
//    givenAPathOfAFile();
//    //givenASlaveProcess();

//    whenHashIsWrittenOnAFile(emptyFileFd,FILE_PATH_TO_HASH);

//    thenFileIsWrittenCorrectly(emptyFileFd);
// }

// void givenAFileDescriptorThatIsEmpty() {
//    return open("testWriteAHashOnAFile.txt",O_CREAT|O_RDWR);
// }

// void givenAPathOfAFile() {
//    int fd = open(FILE_PATH_TO_HASH,O_CREAT|O_RDWR);
//    write(fd,"Text on the file to be hashed.",strlen("Text on the file to be hashed.")+1);
//    close(fd);
// }

// void whenHashIsWrittenOnAFile(int fd, char * filePath) {
//    writeHashOnFd(fd,filePath);
// }

// void writeHashOnFd(int fd, char *filePath) {
//    int status;
//    pid_t pid;
//    pid = fork();
//    if(pid == 0) {
//       close(1);
//       open(fd,O_CREAT|O_RDWR);
//      if(execl("/usr/bin/md5sum","md5sum",filePath,NULL) == -1) {
//             perror("execl Failed.\n");
//             exit(1);
//      }
//    }
//    waitpid(pid,&status,0);
// }

// void thenFileIsWrittenCorrectly(emptyFileFd) {
//      CU_ASSERT(10 == 10);

// }


void testReadAFilePath() {
   int fd;
   char * path;
   fd = givenAFileDescriptorWithSomethingWritten();
   //givenASlaveProcess();
   path = WhenAFilePathIsReadFromFileDescriptor(fd);


   
   thenFilesMustBeTheSame(path);
}

int givenAFileDescriptorWithSomethingWritten() {
   int fd = open("testReadAFilePath.txt", O_CREAT|O_RDWR,777);
   write(fd,FILE_PATH_TO_READ,FILE_PATH_LENGTH);
   lseek(fd,0,SEEK_SET);
   return fd;
}


char * WhenAFilePathIsReadFromFileDescriptor(int fd){
   char * path = getPath(fd);
   close(fd);
   return path;
}

unsigned char * getPath(int fd) {
	unsigned char separator = 0;
	unsigned char * stringToReturn = NULL;
	int index = 0;
	int finished = FALSE;
	int readReturn;

	do{
		if((index % BLOCK) == 0) {
			stringToReturn = realloc(stringToReturn, (index + BLOCK) * sizeof(char));
		}
		readReturn = read(fd,(void *)(stringToReturn + (index * sizeof(char))),1);
		if((readReturn <= 0) || (stringToReturn[index] == EOF) || (stringToReturn[index] == separator)) {
			stringToReturn[index] = 0;
			finished = TRUE;
		}
		index++;
	} while(!finished);
	return stringToReturn;
}

void thenFilesMustBeTheSame(char * path) {
   
  CU_ASSERT(0 == strcmp(path, "./test/PathName/ToRead.txt"));  

}


/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("slaveSuite", initSlaveSuite, cleanSlaveSuite);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   // /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   // if ((NULL == CU_add_test(pSuite, "test of read a file path()", testReadAFilePath))
   //  || (NULL == CU_add_test(pSuite, "test of write the hash of a file on a file()", testWriteAHashOnAFile))
   if ((NULL == CU_add_test(pSuite, "test of read a file path()", testReadAFilePath))) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}