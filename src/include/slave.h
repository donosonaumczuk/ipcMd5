#ifndef SLAVE_H
#define SLAVE_H

#define BLOCK	10
#define TRUE	 1
#define FALSE	 0
#define HASH_MD5_LENGTH 32

char *getPath(int fd);
void writeHashOnFd(int fd, char *filePath);
int givenAFileDescriptorWithSomethingWritten();

#endif