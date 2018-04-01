#ifndef SLAVE_H
#define SLAVE_H

#define BLOCK	10
#define TRUE	 1
#define FALSE	 0
#define HASH_MD5_LENGTH 32
#define MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY 1

char *getPath(int fd);
void writeHashOnFd(int fd, char *filePath);
int givenAFileDescriptorWithSomethingWritten();
void waitForAnswer(int fd);
void readNumber(int fd, char *buffer, int count);
int getNumberOfFilePaths(int fd);

#endif
