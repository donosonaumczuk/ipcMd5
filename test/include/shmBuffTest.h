#ifndef SHMBUFFTEST_H

#define SHMBUFFTEST_H

#include "../../src/include/smhBuff.h"
#include <CUnit/Basic.h>


#define SHM_BUFF_NAME "/shmbuffnametest1"
#define STRING_TO_WRITE "I am written"
#define SHM_BUFF_SIZE 50


int initShmBuffSuite(void);
int cleanShmBuffSuite(void);
void testWriteInShmBuff(void);
ShmBuffCDT givenAShmBuff(void);
void whenWriteInShmBuff(ShmBuffCDT shmBuffPointer);
void thenIsWrittenInShmBuff(ShmBuffCDT shmBuffPointer);

#endif
