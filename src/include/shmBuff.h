#ifndef SHM_BUFF_H

#define SHM_BUFF_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <errno.h>
#include <errors.h>
#include <tools.h>
#include <ipcMd5.h> //evans

#define PID_DEFAULT 0
#define START 0
#define IS_SHARED 1
#define SEM_INIT_VALUE 1
#define OFF_SET 0
#define FALSE 0
#define TRUE 1
#define FAIL -1
#define ONE_CHAR 1
#define BLOCK 10
#define SHM_SEMAPHORE "SharedMemoery Semaphore"
#define BUFFER_SIZE (PATH_MAX + FORMAT_DIGITS)

typedef struct ShmBuff *ShmBuff_t;

ShmBuff_t shmBuffInit(int size, char *shmName);
ShmBuff_t shmBuffAlreadyInit(char const *shmName);
int writeInShmBuff(ShmBuff_t shmBuffPointer, signed char *string, int size);
void readFromShmBuff(ShmBuff_t shmBuffPointer, signed char *buffer, int size);
void freeAndUnmapSharedMemory(ShmBuff_t shmBuffPointer, char *shmName);
void unmapSharedMemory(ShmBuff_t shmBuffPointer, char const *shmName);
void closeSharedMemory(ShmBuff_t shmBuffPointer, char *shmName);
char *getStringFromBuffer(ShmBuff_t shmBuffPointer);

#endif
