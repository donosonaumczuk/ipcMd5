#ifndef SLAVE_H
#define SLAVE_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <errors.h>
#include <tools.h>
#include <ipcMd5.h>

#define MD5SUM "/usr/bin/md5sum"
#define MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY 1

char *getPath(int fd);
void writeHashOnFd(int fd, char *filePath, sem_t *md5Sem);
void writeHashErrorOnFd(int fd, char *filePath, sem_t *md5Sem);
int givenAFileDescriptorWithSomethingWritten();
void waitForAnswer(int fd);
int getNumberOfFilePaths(int fd);
void hashFilesOfGivenPaths(int number, int fdpaths, int fdmd5, sem_t *md5Sem);

#endif
