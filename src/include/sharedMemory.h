#ifndef SHARE_MEMORY
#define SHARE_MEMORY

#include <errors.h>
#include <ipcMd5.h>
#include <linux/limits.h>
#include <tools.h>
#include <sys/select.h>
#include <errno.h>
#include <semaphore.h>

#define SHARED_MEMORY_SIZE  (PATH_MAX + MD5MD5_DIGITS + 1)*4

int initSharedMemory(char *shmName);

#endif
