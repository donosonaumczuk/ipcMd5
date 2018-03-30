#ifndef SLAVE_TEST_H
#define SLAVE_TEST_H

#define FILE_PATH_TO_READ "./test/PathName/ToRead.txt"
#define FILE_PATH_TO_HASH "fileToHash.txt"
#define FILE_QUANTITY 2
#define FILE_PATH_LENGTH 26
#define HASH_MD5_LENGTH 32
#define HASH "9ea4d0ca436512ddc466ed8b549c4f8f"

int initSlaveSuite();
int cleanSlaveSuite();
void testWriteAHashOnAFile();
void givenAFileDescriptorThatIsEmpty(int *fd);
void givenAPathOfAFile();
void whenHashIsWrittenOnAFile(int fd, char *filePath);
void thenFileIsWrittenCorrectly(int emptyFileFd);

void testReadAFilePath();
int givenAFileDescriptorWithSomethingWritten();
char *whenAFilePathIsReadFromFileDescriptor(int fd);
void thenFilesMustBeTheSame(char *path);

void testWriteHashOnAFIFO();
char *givenAFifo();
char *givenThePathOfAFileToHash();
void whenASlaveWritesAHashOnAFifo(char *fifoName, char *filePathToHash);
void thenHashIsWrittenOnAFifo(char *fifoName);
	
#endif