#ifndef APLICATION_PROCESS_H

#define APLICATION_PROCESS_H

#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
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
#define FILE_LOAD 2

// int *makeFileToHashQueues(pid_t *slavePids, int slaveQuantity);
int main(int argc, char const *argv[]);
int getSlaveQuantity(int fileQuantity);
pid_t *makeSlaves(int slaveQuantity, int fdAvailableSlavesQueue,
                  int fdMd5Queue);
void * allocateMemory(size_t bytes);
int makeAvailableSlavesQueue();
int getNumberOfProcessors();
int makeMd5ResultQueue();


#endif
