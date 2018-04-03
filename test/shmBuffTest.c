#include "include/shmBuffTest.h"

struct ShmBuff {
    int first;
    int last;
    int size;
    int long readerPid;
    sem_t sem;
    int isLastOperationWrite;
    signed char *buffer;
};

int initShmBuffSuite() {
    return 0;
}

int cleanShmBuffSuite() {
    return 0;
}

void testWriteInShmBuff() {
    ShmBuff_t shmBuffPointer;

    shmBuffPointer = givenAShmBuff();

    whenWriteInShmBuff(shmBuffPointer);

    thenIsWrittenInShmBuff(shmBuffPointer);

    freeAndUnmapShareMemory(shmBuffPointer, SHM_BUFF_NAME);
}

ShmBuff_t givenAShmBuff() {
    return shmBuffInit(SHM_BUFF_SIZE, SHM_BUFF_NAME);
}

void whenWriteInShmBuff(ShmBuff_t shmBuffPointer) {
    writeInShmBuff(shmBuffPointer, (signed char *)STRING_TO_WRITE,
                   sizeof(STRING_TO_WRITE));
}

void thenIsWrittenInShmBuff(ShmBuff_t shmBuffPointer) {
    CU_ASSERT(strcmp((const char *)shmBuffPointer->buffer, STRING_TO_WRITE)
              == 0);
}

void testReadFromShmBuff() {
    ShmBuff_t shmBuffPointer;
    char buffer[SHM_BUFF_SIZE];

    shmBuffPointer = givenAShmBuffWithData();

    whenReadFromShmBuff(shmBuffPointer, buffer);

    thenDataIsInBuffer(buffer);

    freeAndUnmapShareMemory(shmBuffPointer, SHM_BUFF_NAME);
}

ShmBuff_t givenAShmBuffWithData() {
    ShmBuff_t shmBuffPointer = shmBuffInit(SHM_BUFF_SIZE, SHM_BUFF_NAME);
    writeInShmBuff(shmBuffPointer, (signed char *)STRING_TO_WRITE,
                   sizeof(STRING_TO_WRITE));
    return shmBuffPointer;
}

void whenReadFromShmBuff(ShmBuff_t shmBuffPointer, char *buffer) {
    readFromShmBuff(shmBuffPointer, (signed char *)buffer,
                    sizeof(STRING_TO_WRITE));
}

void thenDataIsInBuffer(char *buffer) {
    CU_ASSERT(strcmp(buffer, STRING_TO_WRITE)==0);
}

void testWriteInShmBuffAfterRead() {
    ShmBuff_t shmBuffPointer;

    shmBuffPointer = givenAShmBuffWithDataAfterRead();

    whenWriteInShmBuff(shmBuffPointer);

    thenIsWrittenInShmBuffAfterRead(shmBuffPointer);

    freeAndUnmapShareMemory(shmBuffPointer, SHM_BUFF_NAME);
}

ShmBuff_t givenAShmBuffWithDataAfterRead() {
    ShmBuff_t shmBuffPointer = givenAShmBuffWithData();
    char buffer[SIZE_TO_READ];
    readFromShmBuff(shmBuffPointer, (signed char *)buffer, SIZE_TO_READ);
    return shmBuffPointer;
}

void thenIsWrittenInShmBuffAfterRead(ShmBuff_t shmBuffPointer) {
    CU_ASSERT(strcmp((const char *)shmBuffPointer->buffer, STRING_AFTER_READ)
              == 0);
}

void testReadInShmBuffAfterReadAndWrite() {
    ShmBuff_t shmBuffPointer;
    char buffer[SHM_BUFF_SIZE];

    shmBuffPointer = givenAShmBuffWithDataAfterReadAndWrite();
    whenReadFromShmBuff(shmBuffPointer, buffer);

    thenDataIsInBuffer(buffer);

    freeAndUnmapShareMemory(shmBuffPointer, SHM_BUFF_NAME);
}

ShmBuff_t givenAShmBuffWithDataAfterReadAndWrite() {
    ShmBuff_t shmBuffPointer = givenAShmBuffWithDataAfterRead();
    char buffer[SIZE_TO_READ];
    whenWriteInShmBuff(shmBuffPointer);
    readFromShmBuff(shmBuffPointer, (signed char *)buffer, 2);
    return shmBuffPointer;
}

void testReadAndWriteDifferentProcess() {
    ShmBuff_t shmBuffPointer;
    int pid = fork();
    char buffer[SIZE_TO_READ];

    shmBuffPointer = givenAShmBuffTwoProces(pid, SHM_BUFF_NAME);

    whenReadAndWriteDifferentProcess(shmBuffPointer, pid, buffer);

    thenReadWhatTheOtherWrote(buffer, pid);

    unmapShareMemory(shmBuffPointer, SHM_BUFF_NAME);
}

ShmBuff_t givenAShmBuffTwoProces(int pid, char *shmName) {
    ShmBuff_t shmBuffPointer;
    if(pid != 0) {
        shmBuffPointer = shmBuffInit(SHM_BUFF_SIZE, shmName);
    } else {
        shmBuffPointer = shmBuffAlreadyInit(shmName);
    }
    return shmBuffPointer;
}

void whenReadAndWriteDifferentProcess(ShmBuff_t shmBuffPointer, int pid,
                                      char *buffer) {
    if(pid != 0) {
        writeInShmBuff(shmBuffPointer, (signed char *)STRING_TO_WRITE,
                       sizeof(STRING_TO_WRITE));
        int status;
        waitpid(pid, &status, WNOHANG);
        exit(0);
    } else {
        readFromShmBuff(shmBuffPointer, (signed char *)buffer,
                        sizeof(STRING_TO_WRITE));
    }
}

void thenReadWhatTheOtherWrote(char *buffer, int pid) {
    if(pid == 0) {
        CU_ASSERT(strcmp(buffer, STRING_TO_WRITE)==0);
    }
}

void testReadStringEOF() {
    ShmBuff_t shmBuffPointer;
    int pid = fork();
    char *answer;

    shmBuffPointer = givenAShmBuffTwoProces(pid, SHM_BUFF_NAME_2);

    answer = whenWriterCloseAndReaderReads(shmBuffPointer, pid);

    thenAnswerIsEOF(answer);

    unmapShareMemory(shmBuffPointer, SHM_BUFF_NAME_2);
}

char *whenWriterCloseAndReaderReads(ShmBuff_t shmBuffPointer, int pid) {
    if(pid != 0) {
        closeShareMemory(shmBuffPointer, SHM_BUFF_NAME);
        int status;
        waitpid(pid, &status, WNOHANG);
        exit(0);
    } else {
        return getStringFromBuffer(shmBuffPointer);
    }
}

void thenAnswerIsEOF(char *answer) {
    CU_ASSERT(answer == (void *)EOF);
}
