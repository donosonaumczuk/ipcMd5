#include "include/smhBuff.h"

struct ShmBuff {
    int first;
    int last;
    int size;
    long int writerPid;
    long int readerPid;
    sem_t sem;
    int isLastOperationWrite;
    char *buffer;
};

//--------------------------------------------------------------------------------
//QUE NO  ME OLVIDE DE LOS ERRORES
//--------------------------------------------------------------------------------

ShmBuffCDT shmBuffInit(int size, char *shmName) {
    int fd;
    if((fd = shm_open(shmName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR))
       == ERROR_STATE) {
        error(OPEN_SHARE_MEMORY_ERROR);
    }

    if(ftruncate(fd, sizeof(size * sizeof(char) + sizeof(struct ShmBuff)))
       == ERROR_STATE) {
        error(TRUNCATE_ERROR);
    }

    ShmBuffCDT shmBuffPointer;
    if((shmBuffPointer = mmap(NULL, size * sizeof(char) + sizeof(struct ShmBuff),
       PROT_READ | PROT_WRITE, MAP_SHARED, fd, OFF_SET)) == ERROR_STATE) {
           error(MAP_ERROR);
    }

    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }

    shmBuffPointer->first = START;
    shmBuffPointer->last = START;
    shmBuffPointer->size = size;
    shmBuffPointer->readerPid = PID_DEFAULT;
    shmBuffPointer->writerPid = PID_DEFAULT;
    sem_init(&shmBuffPointer->sem, IS_SHARE, SEM_INIT_VALUE);
    shmBuffPointer->isLastOperationWrite = FALSE;
    shmBuffPointer->buffer = (char *) (&shmBuffPointer->buffer + sizeof(char));

    return shmBuffPointer;
}

ShmBuffCDT shmBuffAlreadyInit(char *shmName) {
    struct stat stat;
    int fd;
    if((fd = shm_open(shmName,  O_RDWR, S_IRUSR | S_IWUSR)) == ERROR_STATE) {
        error(OPEN_SHARE_MEMORY_ERROR);
    }

    if(fstat(fd, &stat) == ERROR_STATE) {
        error(STAT_ERROR);
    }

    ShmBuffCDT shmBuffPointer;
    if((shmBuffPointer = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE,
       MAP_SHARED, fd, OFF_SET)) == ERROR_STATE) {
        error(MAP_ERROR);
    }

    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }

    return shmBuffPointer;
}

void sleepWriter(ShmBuffCDT shmBuffPointer, int size) {
    int isLastGreaterThanFirst = shmBuffPointer->last >= shmBuffPointer->first;
    int distance = shmBuffPointer->last - shmBuffPointer->first;
    distance = (isLastGreaterThanFirst) ? distance : distance + shmBuffPointer->size;

    if(distance == 0 && shmBuffPointer->isLastOperationWrite){
        distance = shmBuffPointer->size;
    }

    if (distance + size > shmBuffPointer->size) {
        if(sem_wait(&shmBuffPointer->sem) == ERROR_STATE) {
            error(SEMAPHORE_ERROR);
        }

        shmBuffPointer->writerPid = getpid();
        if(kill(shmBuffPointer->writerPid, SIGSTOP) == ERROR_STATE) {
            error(KILL_ERROR);
        }

        if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
            error(SEMAPHORE_ERROR);
        }
    }
}

void wakeupWriter(ShmBuffCDT shmBuffPointer) {
    int isWriterSleep = shmBuffPointer->writerPid;

    if(isWriterSleep) {
        if(kill(shmBuffPointer->writerPid, SIGCONT) == ERROR_STATE) {
            error(KILL_ERROR);
        }
        shmBuffPointer->writerPid = PID_DEFAULT;
    }
}

void sleepReader(ShmBuffCDT shmBuffPointer, int size) {
    int isLastGreaterThanFirst = shmBuffPointer->last >= shmBuffPointer->first;
    int distance = shmBuffPointer->last - shmBuffPointer->first;
    distance = (isLastGreaterThanFirst) ? distance : distance + shmBuffPointer->size;

    if(distance == 0 && shmBuffPointer->isLastOperationWrite){
        distance = shmBuffPointer->size;
    }

    if(distance < size) {
        if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
            error(SEMAPHORE_ERROR);
        }

        shmBuffPointer->readerPid = getpid();
        if(kill(shmBuffPointer->readerPid, SIGSTOP) == ERROR_STATE) {
            error(KILL_ERROR);
        }

        if(sem_wait(&shmBuffPointer->sem) == ERROR_STATE) {
            error(SEMAPHORE_ERROR);
        }
    }
}

void wakeupReader(ShmBuffCDT shmBuffPointer) {
    int isReaderSleep = shmBuffPointer->readerPid;

    if(isReaderSleep) {
        if(kill(shmBuffPointer->readerPid, SIGCONT) == ERROR_STATE) {
            error(KILL_ERROR);
        }
        shmBuffPointer->readerPid = PID_DEFAULT;
    }
}

void writeInShmBuff(ShmBuffCDT shmBuffPointer, char *string, int size) {
    if(sem_wait(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_ERROR);
    }

    sleepWriter(shmBuffPointer, size);

    for (int i = 0; i < size; i++) {
        if(shmBuffPointer->last >= shmBuffPointer->size) {
            shmBuffPointer->last = START;
        }
        shmBuffPointer->buffer[shmBuffPointer->last] = string[i];
        shmBuffPointer->last++;
    }
    shmBuffPointer->isLastOperationWrite = TRUE;

    wakeupReader(shmBuffPointer);

    if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_ERROR);
    }
}

void readFromShmBuff(ShmBuffCDT shmBuffPointer, char *buffer, int size) {
    if(sem_wait(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_ERROR);
    }

    sleepReader(shmBuffPointer, size);

    for (int i = 0; i < size; i++) {
        if(shmBuffPointer->first == shmBuffPointer->size) {
            shmBuffPointer->first = START;
        }
        buffer[i] = shmBuffPointer->buffer[shmBuffPointer->first];
        shmBuffPointer->first++;
    }
    shmBuffPointer->isLastOperationWrite = FALSE;

    wakeupWriter(shmBuffPointer);

    if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_ERROR);
    }
}

void freeAndUnmapShareMemory(ShmBuffCDT shmBuffPointer, char *shmName) {
    unmapShareMemory(shmBuffPointer, shmName);
    if(shm_unlink(shmName) == ERROR_STATE) {
        error(UNLINK_SHARED_MEMORY_ERROR);
    }
}

void unmapShareMemory(ShmBuffCDT shmBuffPointer, char *shmName) {
    struct stat stat;
    int fd;
    if((fd = shm_open(shmName,  O_RDWR, S_IRUSR | S_IWUSR)) == ERROR_STATE) {
        error(OPEN_SHARE_MEMORY_ERROR);
    }

    if(fstat(fd, &stat) == ERROR_STATE) {
        error(STAT_ERROR);
    }

    if(munmap(shmBuffPointer, stat.st_size) == ERROR_STATE) {
        error(UNMAP_ERROR);
    }
    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
}
