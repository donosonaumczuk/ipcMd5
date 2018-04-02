#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <ctype.h>
#include "errors.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stringToInt(char *string);
void * allocateMemory(size_t bytes);
int isValidFilePath(char *filePath);
void intToString(int integer, char *string);

#endif
