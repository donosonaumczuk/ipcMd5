#ifndef APLICATION_PROCESS_H

#define APLICATION_PROCESS_H

#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include "tools.h"
#include "ipcMd5.h"
#include "errors.h"

#define TRUE 1
#define FALSE 0
#define NPROC_BIN_PATH "/usr/bin/nproc"
#define NPROC_BIN_NAME "nproc"
#define SLAVE_BIN_PATH "./slave" //CHECK BIN FILE PATH
#define SLAVE_BIN_NAME "slave"
#define MAX_CORE_DIGITS 3
#define GREATEST_FILE_LOAD 2
#define SMALLEST_FILE_LOAD 1
#define EMPTY -1
#define EQUALS 0
#define VISTA_PROC_FLAG "-v"
#define VISTA_PROC_BIN_PATH "/vistaProcess"
#define VISTA_PROC_BIN_NAME "vistaProcess"

// int *makeFileToHashQueues(pid_t *slavePids, int slaveQuantity);
int main(int argc, char const *argv[]);
int getSlaveQuantity(int fileQuantity);
pid_t *makeSlaves(int slaveQuantity, int fdAvailableSlavesQueue,
                  int fdMd5Queue);
int makeAvailableSlavesQueue();
int getNumberOfProcessors();
int makeMd5ResultQueue();
int readSlavePidString(int fdAvailableSlavesQueue, char *pidString);
void sendNextFile(char *fifoName, char const *filePath);
int getFileLoad(int slaveQuantity, int fileQuantity);
int monitorFds(int maxFd, fd_set *fdSetPointer);
fd_set getFdSet(int fdAvailableSlavesQueue, int fdMd5Queue, int * maxFd);

#endif
