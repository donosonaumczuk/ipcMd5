#include "include/smhBuff.h"

struct ShmBuff {
    int first;
    int last;
    int size;
    sem_t sem;
    int isLastOperationWrite;
    signed char *buffer;
};

ShmBuffCDT shmBuffInit(int size, char *shmName) {
    int fd;
    if((fd = shm_open(shmName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR))
       == ERROR_STATE) {
        error(OPEN_SHARE_MEMORY_ERROR);
    }

    if(ftruncate(fd, sizeof(size * sizeof(signed char) + sizeof(struct ShmBuff)))
       == ERROR_STATE) {
        error(TRUNCATE_ERROR);
    }

    ShmBuffCDT shmBuffPointer;
    if((shmBuffPointer = mmap(NULL, size * sizeof(signed char) + sizeof(struct ShmBuff),
       PROT_READ | PROT_WRITE, MAP_SHARED, fd, OFF_SET)) == ERROR_STATE) {
           error(MAP_ERROR);
    }

    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }

    shmBuffPointer->first = START;
    shmBuffPointer->last = START;
    shmBuffPointer->size = size;
    sem_init(&shmBuffPointer->sem, IS_SHARE, SEM_INIT_VALUE);
    shmBuffPointer->isLastOperationWrite = FALSE;
    shmBuffPointer->buffer = (signed char *) (&shmBuffPointer->buffer + sizeof(signed char));

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

int canWrite(ShmBuffCDT shmBuffPointer, int size) {
    int isLastGreaterThanFirst = shmBuffPointer->last >= shmBuffPointer->first;
    int distance = shmBuffPointer->last - shmBuffPointer->first;
    distance = (isLastGreaterThanFirst) ? distance : distance + shmBuffPointer->size;

    if(distance == 0 && shmBuffPointer->isLastOperationWrite){
        distance = shmBuffPointer->size;
    }

    if (distance + size > shmBuffPointer->size) {
        return FALSE;
    }
    return TRUE;
}

int canRead(ShmBuffCDT shmBuffPointer, int size) {
    int isLastGreaterThanFirst = shmBuffPointer->last >= shmBuffPointer->first;
    int distance = shmBuffPointer->last - shmBuffPointer->first;
    distance = (isLastGreaterThanFirst) ? distance : distance + shmBuffPointer->size;

    if(distance == 0 && shmBuffPointer->isLastOperationWrite){
        distance = shmBuffPointer->size;
    }

    if(distance < size) {
        return FALSE;
    }
    return TRUE;
}

int writeInShmBuff(ShmBuffCDT shmBuffPointer, signed char *string, int size) {
    int answer = SUCCEFULL;

    if(sem_trywait(&shmBuffPointer->sem) == ERROR_STATE) {
        if(errno == EAGAIN) {
            answer = FAIL;
        } else {
            error(SEMAPHORE_ERROR);
        }
    }

    if(canWrite(shmBuffPointer, size)) {
        for (int i = 0; i < size; i++) {
            if(shmBuffPointer->last >= shmBuffPointer->size) {
                shmBuffPointer->last = START;
            }
            shmBuffPointer->buffer[shmBuffPointer->last] = string[i];
            shmBuffPointer->last++;
        }
        shmBuffPointer->isLastOperationWrite = TRUE;
    } else {
        answer = FAIL;
    }

    if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_ERROR);
    }
    return answer;
}

int readFromShmBuff(ShmBuffCDT shmBuffPointer, signed char *buffer, int size) {
    int answer = SUCCEFULL;
    if(sem_trywait(&shmBuffPointer->sem) == ERROR_STATE) {
        if(errno == EAGAIN) {
            answer = FAIL;
        } else {
            error(SEMAPHORE_ERROR);
        }
    }

    if(canRead(shmBuffPointer, size)) {
        for (int i = 0; i < size; i++) {
            if(shmBuffPointer->first == shmBuffPointer->size) {
                shmBuffPointer->first = START;
            }
            buffer[i] = shmBuffPointer->buffer[shmBuffPointer->first];
            shmBuffPointer->first++;
        }
        shmBuffPointer->isLastOperationWrite = FALSE;
    } else {
        answer = FAIL;
    }

    if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_ERROR);
    }
    return answer;
}

void closeShareMemory(ShmBuffCDT shmBuffPointer, char *shmName) {
    signed char eof = EOF;
    writeInShmBuff(shmBuffPointer, &eof, 1);
    freeAndUnmapShareMemory(shmBuffPointer, shmName);
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
