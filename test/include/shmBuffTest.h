#ifndef SHMBUFFTEST_H

#define SHMBUFFTEST_H

#include "../../src/include/smhBuff.h"
#include <CUnit/Basic.h>
#include <sys/wait.h>


#define SHM_BUFF_NAME "/shmbuffnametest"
#define SHM_BUFF_NAME_2 "/shmbuffnametest2"
#define STRING_TO_WRITE "I am written"
#define SHM_BUFF_SIZE 15
#define SIZE_TO_READ 11
#define STRING_AFTER_READ "am written"


int initShmBuffSuite(void);
int cleanShmBuffSuite(void);
void testWriteInShmBuff(void);
ShmBuff_t givenAShmBuff(void);
void whenWriteInShmBuff(ShmBuff_t shmBuffPointer);
void thenIsWrittenInShmBuff(ShmBuff_t shmBuffPointer);
void testReadFromShmBuff(void);
ShmBuff_t givenAShmBuffWithData(void);
void whenReadFromShmBuff(ShmBuff_t shmBuffPointer, char *buffer);
void thenDataIsInBuffer(char *buffer);
void testWriteInShmBuffAfterRead(void);
ShmBuff_t givenAShmBuffWithDataAfterRead(void);
void thenIsWrittenInShmBuffAfterRead(ShmBuff_t shmBuffPointer);
void testReadInShmBuffAfterReadAndWrite(void);
ShmBuff_t givenAShmBuffWithDataAfterReadAndWrite(void);
void testReadAndWriteDifferentProcess(void);
ShmBuff_t givenAShmBuffTwoProces(int pid, char *shmName);
void whenReadAndWriteDifferentProcess(ShmBuff_t shmBuffPointer, int pid,
                                      char *buffer);
void thenReadWhatTheOtherWrote(char *buffer, int pid);
void testReadStringEOF();
char *whenWriterCloseAndReaderReads(ShmBuff_t shmBuffPointer, int pid);
void thenAnswerIsEOF(char *answer);

#endif
