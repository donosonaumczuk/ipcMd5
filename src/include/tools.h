#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <ctype.h>
#include <errors.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLOCK	10
#define TRUE	 1
#define FALSE	 0

int stringToInt(char *string);
void * allocateMemory(size_t bytes);
void * reAllocateMemory(void *currentAddress, size_t bytes);
int isValidFilePath(char *filePath);
void intToString(int integer, char *string);
char *getStringFromFd(int fd, char separator);

#endif
