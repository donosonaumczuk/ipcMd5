#ifndef IPCMD5_H

#define IPCMD5_H

#include <sys/stat.h>
#include <linux/limits.h>

#define MAX_LONG_DIGITS 20
#define MAX_PID_DIGITS 7
#define MD5_DIGITS 32
#define AVAILABLE_SLAVES_QUEUE "availableSlavesQueue"
#define MD5_RESULT_QUEUE "md5ResultQueue"
#define MD5_SEMAPHORE "/md5Semaphore"
#define AVAILABLE_SLAVES_SEMAPHORE "/availableSlavesSemaphore"
#define ONE_RESOURCE 1
#define GREATEST_FILE_LOAD 2
#define SMALLEST_FILE_LOAD 1
#define GREATEST_LOAD_DIGITS 1
#define FORMAT_DIGITS 3

#endif
