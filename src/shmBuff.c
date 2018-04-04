#include <shmBuff.h>

struct ShmBuff {
    int first;
    int last;
    int size;
    int long readerPid;
    sem_t sem;
    int isLastOperationWrite;
     char buffer[BUFFER_SIZE];
};

ShmBuff_t shmBuffInit(char *shmName) {
    int fd;
    if((fd = shm_open(shmName, O_CREAT | O_RDWR, 0777)) //evans
       == ERROR_STATE) {
        error(OPEN_SHARED_MEMORY_ERROR);
    }

    if(ftruncate(fd, sizeof(struct ShmBuff)) == ERROR_STATE) {
        error(TRUNCATE_ERROR);
    }

    ShmBuff_t shmBuffPointer;
    if((shmBuffPointer = mmap(NULL,
       sizeof(struct ShmBuff), PROT_READ | PROT_WRITE, MAP_SHARED, fd, OFF_SET))
       == (void *)ERROR_STATE) {
           error(MAP_ERROR);
    }

    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }

    shmBuffPointer->first = START;
    shmBuffPointer->last = START;
    shmBuffPointer->size = BUFFER_SIZE;
    shmBuffPointer->readerPid = PID_DEFAULT;
    sem_init(&shmBuffPointer->sem, IS_SHARED, SEM_INIT_VALUE);
    shmBuffPointer->isLastOperationWrite = FALSE;
    //shmBuffPointer->buffer = ( char *)(&shmBuffPointer->buffer +
                             // sizeof( char));

    return shmBuffPointer;
}

ShmBuff_t shmBuffAlreadyInit(char const *shmName) {
    struct stat stat;
    int fd;
    if((fd = shm_open(shmName,  O_RDWR, 0777)) == ERROR_STATE) {
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

int canWrite(ShmBuff_t shmBuffPointer, int size) {
    int isLastGreaterThanFirst = shmBuffPointer->last >= shmBuffPointer->first;
    int distance = shmBuffPointer->last - shmBuffPointer->first;
    distance = (isLastGreaterThanFirst) ? distance : distance +
                shmBuffPointer->size;

    if(distance == 0 && shmBuffPointer->isLastOperationWrite){
        distance = shmBuffPointer->size;
    }

    if (distance + size > shmBuffPointer->size) {
        return FALSE;
    }
    return TRUE;
}

void sleepReader(ShmBuff_t shmBuffPointer, int size) {
    int isLastGreaterThanFirst = shmBuffPointer->last >= shmBuffPointer->first;
    int distance = shmBuffPointer->last - shmBuffPointer->first;
    distance = (isLastGreaterThanFirst) ? distance :
                distance + shmBuffPointer->size;

    if(distance == 0 && shmBuffPointer->isLastOperationWrite){
        distance = shmBuffPointer->size;
    }

    if(distance < size) {
        if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
            error(SEMAPHORE_POST_ERROR(SHM_SEMAPHORES));
        }

        shmBuffPointer->readerPid = getpid();
        if(kill(shmBuffPointer->readerPid, SIGSTOP) == ERROR_STATE) {
            error(KILL_ERROR);
        }
        printf("\ta dormir\n"); //evans

        if(sem_wait(&shmBuffPointer->sem) == ERROR_STATE) {
            error(SEMAPHORE_WAIT_ERROR(SHM_SEMAPHORES));
        }
    }
}

void wakeupReader(ShmBuff_t shmBuffPointer) {
    int isReaderSleep = shmBuffPointer->readerPid;

    if(isReaderSleep) {
        if(kill(shmBuffPointer->readerPid, SIGCONT) == ERROR_STATE) {
            error(KILL_ERROR);
        }
        printf("kill to wake up reader executed -> readerPid = %ld\n", shmBuffPointer->readerPid); //evans
        shmBuffPointer->readerPid = PID_DEFAULT;
    }
}

int writeInShmBuff(ShmBuff_t shmBuffPointer,  char *string, int size) {
    int answer = OK_STATE;

    if(sem_trywait(&shmBuffPointer->sem) == ERROR_STATE) {
        if(errno == EAGAIN) {
            printf("EAGAIN\n"); //evans
            answer = FAIL;
        }
        else {
            error(SEMAPHORE_WAIT_ERROR(SHM_SEMAPHORES));
        }
    }

    if((answer == OK_STATE) && canWrite(shmBuffPointer, size)) {
        for (int i = 0; i < size; i++) {
            if(shmBuffPointer->last >= shmBuffPointer->size) {
                shmBuffPointer->last = START;
            }
            shmBuffPointer->buffer[shmBuffPointer->last] = string[i];
            shmBuffPointer->last++;
        }
        shmBuffPointer->isLastOperationWrite = TRUE;
    }
    else {
        answer = FAIL;
    }

    if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(SHM_SEMAPHORES));
    }

    if(answer == OK_STATE) {
        wakeupReader(shmBuffPointer);
    }

    return answer;
}

void readFromShmBuff(ShmBuff_t shmBuffPointer,  char *buffer, int size) {
    if(sem_wait(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(SHM_SEMAPHORES));
    }

    sleepReader(shmBuffPointer, size);
    printf("Continue execution (can be by a wake up)\n"); //evans

    for (int i = 0; i < size; i++) {
        if(shmBuffPointer->first == shmBuffPointer->size) {
            shmBuffPointer->first = START;
        }

        buffer[i] = shmBuffPointer->buffer[shmBuffPointer->first];
        shmBuffPointer->first++;
    }
    shmBuffPointer->isLastOperationWrite = FALSE;

    if(sem_post(&shmBuffPointer->sem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(SHM_SEMAPHORES));
    }
}

void closeSharedMemory(ShmBuff_t shmBuffPointer, char *shmName) {
    writeInShmBuff(shmBuffPointer, "", 1);
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

char *getStringFromBuffer(ShmBuff_t shmBuffPointer) {
    int i = 0, flag = TRUE, size = 0;
    char current;
    char *buffer = NULL;

    do {
        if(i % BLOCK == 0) {
            size += BLOCK;
            buffer = (char *) reAllocateMemory(buffer, size);
            printf("REALLOC\n"); //evans
        }
        printf("Will read from shmBuff a char\n"); //evans
        readFromShmBuff(shmBuffPointer, &current, ONE_CHAR);
        printf("Read a char from shmBuff\n"); //evans
        if (current == EOF) {
            buffer = (char *) NULL;
            flag = FALSE;
        }
        else {
            if(current == 0) {
                flag = FALSE;
            }
            buffer[i++] = current;
        }

    } while (flag);

    printf("Exit getStringFromBuffer char by char\n"); //evans

    return buffer;
}
