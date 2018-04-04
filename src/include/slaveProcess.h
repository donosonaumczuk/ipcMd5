#ifndef SLAVE_PROCESS_H
#define SLAVE_PROCESS_H

#define _GNU_SOURCE

#include <fcntl.h>
#include <slave.h>
#include <ipcMd5.h>
#include <applicationProcess.h>
#include <tools.h>

int createFilePathFifo(char *name, int fdrequest, sem_t *availableSlavesSem);

#endif
