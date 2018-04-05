#include <shmBuff.h>

struct ShmBuff {
    int first;
    int last;
    int size;
    sem_t sem;
    char buffer[BUFFER_SIZE];
};

ShmBuff_t shmBuffInit(char *shmName) {
    int fd;
    if((fd = shm_open(shmName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR))
       == ERROR_STATE) {
        error(OPEN_SHARED_MEMORY_ERROR);
    }

    if(ftruncate(fd, sizeof(struct ShmBuff)) == ERROR_STATE) {
        error(TRUNCATE_ERROR);
    }

    ShmBuff_t shmBuffPointer;
    if((shmBuffPointer = mmap(NULL, sizeof(struct ShmBuff), PROT_READ |
        PROT_WRITE, MAP_SHARED, fd, OFF_SET)) == (void *)ERROR_STATE) {
           error(MAP_ERROR);
    }

    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }

    shmBuffPointer->first = START;
    shmBuffPointer->last = START;
    shmBuffPointer->size = BUFFER_SIZE;
    sem_init(&shmBuffPointer->sem, IS_SHARED, SEM_INIT_VALUE);

    return shmBuffPointer;
}

ShmBuff_t shmBuffAlreadyInit(char const *shmName) {
    struct stat stat;
    int fd;
    if((fd = shm_open(shmName,  O_RDWR, S_IRUSR | S_IWUSR)) == ERROR_STATE) {
        error(OPEN_SHARED_MEMORY_ERROR);
    }

    if(fstat(fd, &stat) == ERROR_STATE) {
        error(STAT_ERROR);
    }

    ShmBuff_t shmBuffPointer;
    if((shmBuffPointer = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE,
       MAP_SHARED, fd, OFF_SET)) == (void *)ERROR_STATE) {
        error(MAP_ERROR);
    }

    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }

    return shmBuffPointer;
}

void writeInShmBuff(ShmBuff_t shmBuffPointer, sem_t *empty, sem_t *full,
                    char buffer) {
    printf("full wait writeInShmBuff\n"); //evans

    if(sem_wait(full) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(SHM_SEMAPHORES));
    }
    printf("sem wait writeInShmBuff\n"); //evans

    if(sem_wait(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(SHM_SEMAPHORES));
    }

    if(shmBuffPointer->last >= shmBuffPointer->size) {
        shmBuffPointer->last = START;
    }
    shmBuffPointer->buffer[shmBuffPointer->last] = buffer;
    shmBuffPointer->last++;

    if(sem_post(empty) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(SHM_SEMAPHORES));
    }
    printf("empty post writeInShmBuff\n"); //evans

    if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(SHM_SEMAPHORES));
    }
    printf("sem post writeInShmBuff\n"); //evans

}

void readFromShmBuff(ShmBuff_t shmBuffPointer, sem_t *empty, sem_t *full,
                     char *buffer) {
     printf("empty wait readFromShmBuff\n"); //evans
     if(sem_wait(empty) == ERROR_STATE) {
         error(SEMAPHORE_WAIT_ERROR(SHM_SEMAPHORES));
     }
    printf("sem wait readFromShmBuff\n"); //evans
    if(sem_wait(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(SHM_SEMAPHORES));
    }

    if(shmBuffPointer->first == shmBuffPointer->size) {
        shmBuffPointer->first = START;
    }

    *buffer = shmBuffPointer->buffer[shmBuffPointer->first];
    shmBuffPointer->first++;

    if(sem_post(full) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(SHM_SEMAPHORES));
    }
    printf("full post readFromShmBuff\n"); //evans

    if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(SHM_SEMAPHORES));
    }
    printf("sem post readFromShmBuff\n"); //evans

}

void closeSharedMemory(ShmBuff_t shmBuffPointer, sem_t *emptySem,
                       sem_t *fullSem, char *shmName) {
    writeInShmBuff(shmBuffPointer, emptySem, fullSem, (char) 0);
    freeAndUnmapSharedMemory(shmBuffPointer, shmName);
}

void freeAndUnmapSharedMemory(ShmBuff_t shmBuffPointer, char *shmName) {
    unmapSharedMemory(shmBuffPointer, shmName);
    if(shm_unlink(shmName) == ERROR_STATE) {
        error(UNLINK_SHARED_MEMORY_ERROR);
    }
}

void unmapSharedMemory(ShmBuff_t shmBuffPointer, char const *shmName) {
    if(munmap(shmBuffPointer, sizeof(struct ShmBuff)) == ERROR_STATE) {
        error(UNMAP_ERROR);
    }
}

char *getStringFromBuffer(ShmBuff_t shmBuffPointer, sem_t *empty,
                          sem_t *full) {
    int i = 0, flag = TRUE, size = 0;
    char current;
    char *buffer = NULL;

    do {
        if(i % BLOCK == 0) {
            size += BLOCK;
            buffer = (char *) reAllocateMemory(buffer, size);
        }

        readFromShmBuff(shmBuffPointer, empty, full, &current);

        if(current == 0) {
            flag = FALSE;
        }
        
        buffer[i++] = current;

    } while (flag);

    return buffer;
}

void writeStringToShmBuff(ShmBuff_t shmBuffPointer, sem_t *empty, sem_t *full,
                          char *string) {
    int i = 0;
    while(string[i] != 0) {
        writeInShmBuff(shmBuffPointer, empty, full, string[i]);
        i++;
    }
    writeInShmBuff(shmBuffPointer, empty, full, 0);
}
