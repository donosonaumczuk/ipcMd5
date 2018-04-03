#ifndef SLAVE_PROCESS_H
#define SLAVE_PROCESS_H

#include </usr/include/linux/fcntl.h>
#include "slave.h"
#include "ipcMd5.h"
#include "applicationProcess.h"

#define REQUEST_SEMAPHORE "/requestSemaphore"

void createFilePathFifo(char *name, int fdrequest, sem_t *requestSem);

#endif