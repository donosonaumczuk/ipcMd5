#ifndef APPLICATION_PROCESS_H

#define APPLICATION_PROCESS_H

#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <tools.h>
#include <ipcMd5.h>
#include <errors.h>
#include <sys/select.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define NPROC_BIN_PATH "/usr/bin/nproc"
#define NPROC_BIN_NAME "nproc"
#define SLAVE_BIN_PATH "./slaveProcess"
#define SLAVE_BIN_NAME "slaveProcess"
#define MAX_CORE_DIGITS 3
#define EMPTY -1
#define EQUALS 0
#define VIEW_PROC_FLAG "-v"
#define VIEW_PROC_BIN_PATH "/viewProcess"
#define VIEW_PROC_BIN_NAME "viewProcess"

int main(int argc, char const *argv[]);
int getSlaveQuantity(int fileQuantity);
void makeSlaves(int slaveQuantity, int fdAvailableSlavesQueue,
                  int fdMd5Queue);
int makeAvailableSlavesQueue(int slaveQuantity);
int getNumberOfProcessors();
int makeMd5ResultQueue();
int readSlavePidString(int fdAvailableSlavesQueue, char *pidString);
void sendToSlaveFileQueue(char *fifoName, char const *filePath);
int getFileLoad(int slaveQuantity, int fileQuantity);
int monitorFds(int maxFd, fd_set *fdSetPointer);
fd_set getFdSetAvlbAndMd5Queues(int fdAvailableSlavesQueue,
                                int fdMd5Queue, int * maxFd);

#endif
