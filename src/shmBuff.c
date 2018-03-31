#include "include/smhBuff.h"

struct ShmBuff {
    int first;
    int last;
    int size;
    long int writerPid;
    long int readerPid;
    sem_t sem;
    char *buffer;
};

//--------------------------------------------------------------------------------
//QUE NO  ME OLVIDE DE LOS ERRORES
//--------------------------------------------------------------------------------

ShmBuffCDT shmBuffInit(int size, char *shmName) {
    int fd = shm_open(shmName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, sizeof(size * sizeof(char) + sizeof(struct ShmBuff)));
    ShmBuffCDT shmBuffPointer = mmap(NULL, size * sizeof(char) + sizeof(struct ShmBuff),
                                     PROT_READ | PROT_WRITE, MAP_SHARED, fd, OFF_SET);
    close(fd);

    shmBuffPointer->first = START;
    shmBuffPointer->last = START;
    shmBuffPointer->size = size;
    shmBuffPointer->readerPid = PID_DEFAULT;
    shmBuffPointer->writerPid = PID_DEFAULT;
    sem_init(&shmBuffPointer->sem, IS_SHARE, SEM_INIT_VALUE);
    shmBuffPointer->buffer = (char *) (&shmBuffPointer->buffer + sizeof(char));

    return shmBuffPointer;
}

ShmBuffCDT shmBuffAlreadyInit(char *shmName) {
    struct stat stat;
    int fd = shm_open(shmName, O_RDWR, 0);

    fstat(fd,&stat);
    ShmBuffCDT shmBuffPointer = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd, OFF_SET);
    close(fd);

    return shmBuffPointer;
}

void sleepWriter(ShmBuffCDT shmBuffPointer, int size) {
    int isLastGreaterThanFirst = shmBuffPointer->last >= shmBuffPointer->first;
    int distance = shmBuffPointer->last - shmBuffPointer->first;
    distance = (isLastGreaterThanFirst) ? distance : distance + shmBuffPointer->size;

    if (distance + size >= shmBuffPointer->size) {
        shmBuffPointer->writerPid = getpid();
        kill(shmBuffPointer->writerPid, SIGSTOP);
    }
}

void wakeupWriter(ShmBuffCDT shmBuffPointer) {
    int isWriterSleep = shmBuffPointer->writerPid;
    if(isWriterSleep) {
        kill(shmBuffPointer->writerPid, SIGCONT);
        shmBuffPointer->writerPid = PID_DEFAULT;
    }
}

void sleepReader(ShmBuffCDT shmBuffPointer, int size) {
    int isLastGreaterThanFirst = shmBuffPointer->last >= shmBuffPointer->first;
    int distance = shmBuffPointer->last - shmBuffPointer->first;
    distance = (isLastGreaterThanFirst) ? distance : distance + shmBuffPointer->size;

    if(distance < size) {
        shmBuffPointer->readerPid = getpid();
        kill(shmBuffPointer->readerPid, SIGSTOP);
    }
}

void wakeupReader(ShmBuffCDT shmBuffPointer) {
    int isReaderSleep = shmBuffPointer->readerPid;
    if(isReaderSleep) {
        kill(shmBuffPointer->readerPid, SIGCONT);
        shmBuffPointer->readerPid = PID_DEFAULT;
    }
}

void writeInShmBuff(ShmBuffCDT shmBuffPointer, char *string, int size) {
    sleepWriter(shmBuffPointer, size);

    sem_wait(&shmBuffPointer->sem);

    for (int i = 0; i < size; i++) {
        if(shmBuffPointer->last >= shmBuffPointer->size) {
            shmBuffPointer->last = START;
        }

        shmBuffPointer->buffer[shmBuffPointer->last] = string[i];
        shmBuffPointer->last++;
    }

    sem_post(&shmBuffPointer->sem);

    wakeupReader(shmBuffPointer);
}

void readFromShmBuff(ShmBuffCDT shmBuffPointer, char *buffer, int size) {
    sleepReader(shmBuffPointer, size);

    sem_wait(&shmBuffPointer->sem);
    for (int i = 0; i < size; i++) {
        if(shmBuffPointer->first == shmBuffPointer->size) {
            shmBuffPointer->first = START;
        }
        buffer[i] = shmBuffPointer->buffer[shmBuffPointer->first];
        shmBuffPointer->first++;
    }

    sem_post(&shmBuffPointer->sem);

    wakeupWriter(shmBuffPointer);
}
