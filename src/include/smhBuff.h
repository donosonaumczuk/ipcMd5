#ifndef SHMBUFF_H

#define SHMBUFF_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include "errors.h"

#define PID_DEFAULT 0
#define START 0
#define IS_SHARE 1
#define SEM_INIT_VALUE 1
#define OFF_SET 0
#define FALSE 0
#define TRUE 1
#define EOF -1

typedef struct ShmBuff *ShmBuffCDT;

ShmBuffCDT shmBuffInit(int size, char *shmName);
ShmBuffCDT shmBuffAlreadyInit(char *shmName);
void writeInShmBuff(ShmBuffCDT shmBuffPointer, signed char *string, int size);
void readFromShmBuff(ShmBuffCDT shmBuffPointer, signed char *buffer, int size);
void freeAndUnmapShareMemory(ShmBuffCDT shmBuffPointer, char *shmName);
void unmapShareMemory(ShmBuffCDT shmBuffPointer, char *shmName);
void closeShareMemory(ShmBuffCDT shmBuffPointer, char *shmName);

#endif
