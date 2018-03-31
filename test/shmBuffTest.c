#include "include/shmBuffTest.h"

struct ShmBuff {
    int first;
    int last;
    int size;
    long int writerPid;
    long int readerPid;
    sem_t sem;
    char *buffer;
};

int initShmBuffSuite() {
    return 0;
}

int cleanShmBuffSuite() {
    return 0;
}

void testWriteInShmBuff() {
    ShmBuffCDT shmBuffPointer;

    shmBuffPointer = givenAShmBuff();

    whenWriteInShmBuff(shmBuffPointer);

    thenIsWrittenInShmBuff(shmBuffPointer);

    shm_unlink(SHM_BUFF_NAME);
}

ShmBuffCDT givenAShmBuff() {
    return shmBuffInit(SHM_BUFF_SIZE, SHM_BUFF_NAME);
}

void whenWriteInShmBuff(ShmBuffCDT shmBuffPointer) {
    writeInShmBuff(shmBuffPointer, STRING_TO_WRITE, sizeof(STRING_TO_WRITE));
}

void thenIsWrittenInShmBuff(ShmBuffCDT shmBuffPointer) {
    CU_ASSERT(strcmp(shmBuffPointer->buffer, STRING_TO_WRITE)==0);
}

void testReadFromShmBuff() {
    ShmBuffCDT shmBuffPointer;
    char buffer[SHM_BUFF_SIZE];

    shmBuffPointer = givenAShmBuffWithData();

    whenReadFromShmBuff(shmBuffPointer, buffer);

    thenDataIsInBuffer(buffer);
}

ShmBuffCDT givenAShmBuffWithData() {
    ShmBuffCDT shmBuffPointer = shmBuffInit(SHM_BUFF_SIZE, SHM_BUFF_NAME);
    writeInShmBuff(shmBuffPointer, STRING_TO_WRITE, sizeof(STRING_TO_WRITE));
    return shmBuffPointer;
}

void whenReadFromShmBuff(ShmBuffCDT shmBuffPointer, char *buffer) {
    readFromShmBuff(shmBuffPointer, buffer, sizeof(STRING_TO_WRITE));
}

void thenDataIsInBuffer(char *buffer) {
    CU_ASSERT(strcmp(buffer, STRING_TO_WRITE)==0);
}
