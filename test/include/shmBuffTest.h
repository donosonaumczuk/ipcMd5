#ifndef SHMBUFFTEST_H

#define SHMBUFFTEST_H

#include "../../src/include/smhBuff.h"
#include <CUnit/Basic.h>
#include <sys/wait.h>


#define SHM_BUFF_NAME "/shmbuffnametest0"
#define STRING_TO_WRITE "I am written"
#define SHM_BUFF_SIZE 15
#define SIZE_TO_READ 11
#define STRING_AFTER_READ "am written"


int initShmBuffSuite(void);
int cleanShmBuffSuite(void);
void testWriteInShmBuff(void);
ShmBuffCDT givenAShmBuff(void);
void whenWriteInShmBuff(ShmBuffCDT shmBuffPointer);
void thenIsWrittenInShmBuff(ShmBuffCDT shmBuffPointer);
void testReadFromShmBuff(void);
ShmBuffCDT givenAShmBuffWithData(void);
void whenReadFromShmBuff(ShmBuffCDT shmBuffPointer, char *buffer);
void thenDataIsInBuffer(char *buffer);
void testWriteInShmBuffAfterRead(void);
ShmBuffCDT givenAShmBuffWithDataAfterRead(void);
void thenIsWrittenInShmBuffAfterRead(ShmBuffCDT shmBuffPointer);
void testReadInShmBuffAfterReadAndWrite(void);
ShmBuffCDT givenAShmBuffWithDataAfterReadAndWrite(void);
void testReadAndWriteDifferentProcess(void);
ShmBuffCDT givenAShmBuffTwoProces(int pid);
void whenReadAndWriteDifferentProcess(ShmBuffCDT shmBuffPointer, int pid, char *buffer);
void thenReadWhatTheOtherWrote(char *buffer, int pid);

#endif
