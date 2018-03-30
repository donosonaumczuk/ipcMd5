#ifndef SLAVE_TEST_H
#define SLAVE_TEST_H

#define FILE_PATH_TO_READ "./test/PathName/ToRead.txt"
#define FILE_PATH_TO_HASH "FileToHash.txt"
#define FILE_QUANTITY 2
#define FILE_PATH_LENGTH 26

int initSlaveSuite();
int cleanSlaveSuite();
void testWriteAHashOnAFile();
void givenAFileDescriptorThatIsEmpty(int *fd);
void givenAPathOfAFile();
void whenHashIsWrittenOnAFile(int fd, char *filePath);
void thenFileIsWrittenCorrectly(int emptyFileFd);
void testReadAFilePath();
int givenAFileDescriptorWithSomethingWritten();
char *WhenAFilePathIsReadFromFileDescriptor(int fd);
void thenFilesMustBeTheSame(char *path);

#endif