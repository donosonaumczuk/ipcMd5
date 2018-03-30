#ifndef SLAVE_H
#define SLAVE_H

#define BLOCK	10
#define TRUE	 1
#define FALSE	 0

char *getPath(int fd);
void writeHashOnFd(int fd, char *filePath);
int givenAFileDescriptorWithSomethingWritten();

#endif