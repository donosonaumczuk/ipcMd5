#ifndef SLAVE_PROCESS_H
#define SLAVE_PROCESS_H

#include "slave.h"
#include "ipcMd5.h"

#define REQUEST_SEMAPHORE "/requestSemaphore"

void createFilePathFifo(char *name, int fdrequest, sem_t *requestSem);

#endif