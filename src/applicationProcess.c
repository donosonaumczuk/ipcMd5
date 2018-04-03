#include <applicationProcess.h>

int main(int argc, char const *argv[]) {
    if(argc < 2) {
        errorToStderr(INVALID_NUMBER_ARGS_ERROR);
    }
    else {
        pid_t applicationPid = getpid();
        int viewIsSet = FALSE;
        int fileQuantity = argc - 1;
        int nextFileIndex = 1;

        if(strcmp(argv[1], VIEW_PROC_FLAG) == EQUALS) {
            viewIsSet = TRUE;
            pid_t pid;
            if((pid = fork()) == ERROR_STATE) {
                error(FORK_ERROR);
            }

            if(pid == 0) {
                char pidArgument[MAX_PID_DIGITS];
                intToString(applicationPid, pidArgument);
                if(execl(VIEW_PROC_BIN_PATH, VIEW_PROC_BIN_NAME, pidArgument,
                   NULL) == ERROR_STATE) {
                    error(EXEC_ERROR(VIEW_PROC_BIN_PATH));
                }
            }

            nextFileIndex = 2;
            fileQuantity = argc - 2;
        }

        FILE *resultFile = fopen(MD5_RESULT_FILE, WRITE_PERMISSION);

        ShmBuff_t sharedMemory;
        char shmName[MAX_PID_DIGITS];
        if(viewIsSet) {
            intToString(applicationPid, shmName);
            sharedMemory = shmBuffInit((fileQuantity / 4) * (PATH_MAX +
                                       MD5_DIGITS + FORMAT_DIGITS), shmName);
        }

        int slaveQuantity = getSlaveQuantity(fileQuantity);

        int fileLoad = getFileLoad(slaveQuantity, fileQuantity);

        int fdMd5Queue = makeMd5ResultQueue();

        int fdAvailableSlavesQueue = makeAvailableSlavesQueue(slaveQuantity);

        sem_t *availableSlavesSem;
        sem_t *md5QueueSem;
        openSemaphores(&availableSlavesSem, &md5QueueSem);

        makeSlaves(slaveQuantity, fdAvailableSlavesQueue, fdMd5Queue);

        int remainingFiles = fileQuantity;


        int maxFd;
        fd_set fdSetBackup = getFdSetAvlbAndMd5Queues(fdAvailableSlavesQueue,
                                                      fdMd5Queue, &maxFd);
        fd_set fdSet;

        int canReadMd5Queue = TRUE;
        char *md5ResultBuffer;

        int resultsWrited = 0;
        char pidString[MAX_PID_DIGITS + 1];
        int filesToSentQuantity = fileLoad;
        char filesToSentQuantityString[GREATEST_LOAD_DIGITS + 1];

        while(remainingFiles > 0 || resultsWrited < fileQuantity) {
            fdSet = fdSetBackup;
            monitorFds(maxFd + 1, &fdSet);

            if(FD_ISSET(fdAvailableSlavesQueue, &fdSet) && remainingFiles > 0) {
                while(readSlavePidString(fdAvailableSlavesQueue, pidString) !=
                      EMPTY) {
                    if(fileLoad > remainingFiles) {
                        filesToSentQuantity = remainingFiles;
                    }

                    intToString(filesToSentQuantity, filesToSentQuantityString);
                    sendToSlaveFileQueue(pidString, filesToSentQuantityString);

                    for(int i = 0; i < filesToSentQuantity; i++) {
                        sendToSlaveFileQueue(pidString, argv[nextFileIndex++]);
                        remainingFiles--;
                    }
                }
            }

            if(FD_ISSET(fdMd5Queue, &fdSet)) {
                if(canReadMd5Queue) {
                    md5ResultBuffer = getMd5QueueResult(fdMd5Queue,
                                                        md5QueueSem);
                }

                if(viewIsSet) {
                    if (writeInShmBuff(sharedMemory,
                                      (signed char *) md5ResultBuffer,
                                       strlen(md5ResultBuffer) + 1) !=
                                       OK_STATE) {
                        canReadMd5Queue = FALSE;
                    }
                    else {
                        canReadMd5Queue = TRUE;
                        fprintf(resultFile, "%s\n", md5ResultBuffer);
                        free(md5ResultBuffer);
                        resultsWrited++;
                    }
                }
                else {
                    fprintf(resultFile, "%s\n", md5ResultBuffer);
                    free(md5ResultBuffer);
                    resultsWrited++;
                }
            }
        }

        maxFd = fdAvailableSlavesQueue;
        fdSetBackup = getFdSetAvlbQueue(fdAvailableSlavesQueue);

        int finishRequestedSlaves = 0;
        while(finishRequestedSlaves < slaveQuantity) {
            fdSet = fdSetBackup;
            monitorFds(maxFd + 1, &fdSet);

            readSlavePidString(fdAvailableSlavesQueue, pidString);
            intToString(0, filesToSentQuantityString);
            sendToSlaveFileQueue(pidString, filesToSentQuantityString);
            finishRequestedSlaves++;
        }


        int status;
        int finishedProcesses = 0;
        int childrenProcesses = slaveQuantity;

        if(viewIsSet) {
            childrenProcesses++;
        }

        while(finishedProcesses < childrenProcesses) {
            wait(&status);
            finishedProcesses++;
        }

        fclose(resultFile);

        if(close(fdMd5Queue) == ERROR_STATE) {
            error(CLOSE_ERROR);
        }

        if(close(fdAvailableSlavesQueue) == ERROR_STATE) {
            error(CLOSE_ERROR);
        }

        if(viewIsSet) {
            closeSharedMemory(sharedMemory, shmName);
        }
    }

    return 0;
}

void finishSemaphores(sem_t *availableSlavesSem, sem_t *md5QueueSem) {
    if(sem_close(availableSlavesSem) == ERROR_STATE) {
        error(SEMAPHORE_CLOSE_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
    }

    if(sem_close(md5QueueSem) == ERROR_STATE) {
        error(SEMAPHORE_CLOSE_ERROR(MD5_SEMAPHORE));
    }

    unlinkSemaphores();
}

void unlinkSemaphores() {
    if(sem_unlink(AVAILABLE_SLAVES_SEMAPHORE) == ERROR_STATE) {
        error(SEMAPHORE_UNLINK_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
    }

    if(sem_unlink(MD5_SEMAPHORE) == ERROR_STATE) {
        error(SEMAPHORE_UNLINK_ERROR(MD5_SEMAPHORE));
    }
}

char *getMd5QueueResult(int fdMd5Queue, sem_t *md5QueueSemaphore) {
    if(sem_wait(md5QueueSemaphore) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(MD5_SEMAPHORE));
    }

    char *md5Result = getStringFromFd(fdMd5Queue, 0);

    if(sem_post(md5QueueSemaphore) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(MD5_SEMAPHORE));
    }

    return md5Result;
}

void openSemaphores(sem_t **availableSlavesSem, sem_t **md5QueueSem) {
    unlinkSemaphores();

    *availableSlavesSem = sem_open(AVAILABLE_SLAVES_SEMAPHORE, O_CREAT | O_RDWR,
                                   S_IRUSR | S_IWUSR, ONE_RESOURCE);
    if(*availableSlavesSem == SEM_FAILED) {
        error(SEMAPHORE_OPEN_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
    }

    *md5RQueueSem = sem_open(MD5_SEMAPHORE, O_CREAT | O_RDWR,
                               S_IRUSR | S_IWUSR, ONE_RESOURCE);

    if(*md5QueueSem == SEM_FAILED) {
        error(SEMAPHORE_OPEN_ERROR(MD5_SEMAPHORE);
    }
}


fd_set getFdSetAvlbQueue(int fdAvailableSlavesQueue) {
    fd_set fdSet;

    FD_ZERO(&fdSet);
    FD_SET(fdAvailableSlavesQueue, &fdSet);

    return fdSet;
}

int readSlavePidString(int fdAvailableSlavesQueue, char *pidString,
                       sem_t *availableSlavesSem) {
    if(sem_wait(availableSlavesSem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
    }

    if(read(fdAvailableSlavesQueue, pidString, 1) == ERROR_STATE) {
        if(errno == EAGAIN) {
            return EMPTY;
        }

        error(READ_ERROR);
    }

    int index = 0;
    while(pidString[index++] != 0) {
        if(read(fdAvailableSlavesQueue, pidString + index, 1) == ERROR_STATE) {
            error(READ_ERROR);
        }
    }

    if(sem_post(availableSlavesSem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
    }

    return OK_STATE;
}

void sendToSlaveFileQueue(char *pidString, char const *filePath) {
    int fd;
    if((fd = open(pidString, O_WRONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(pidString));
    }

    int semNameSize = strlen(pidString) + 2;
    char semName[semNameSize] = {0};

    strcat(semName, "/");
    strcat(semName, pidString);

    sem_t *slaveFileQueueSem = sem_open(semName, O_WRONLY);
    if(slaveFileQueueSem == SEM_FAILED) {
        error(SEMAPHORE_OPEN_ERROR(semName);
    }

    if(sem_wait(slaveFileQueueSem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(semName));
    }

    if(write(fd, filePath, strlen(filePath) + 1) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(filePath));
    }

    if(sem_post(slaveFileQueueSem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(semName));
    }

    if(sem_close(slaveFileQueueSem) == ERROR_STATE) {
        error(SEMAPHORE_CLOSE_ERROR(semName));
    }

    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
}

int getFileLoad(int slaveQuantity, int fileQuantity) {
    int fileLoad = GREATEST_FILE_LOAD;

    if(slaveQuantity == fileQuantity) {
        fileLoad = SMALLEST_FILE_LOAD;
    }

    return fileLoad;
}

int monitorFds(int maxFd, fd_set *fdSetPointer) {
    int r;
    if((r = select(maxFd + 1, fdSetPointer, NULL, NULL, NULL)) == ERROR_STATE) {
        error(SELECT_ERROR);
    }

    return r;
}

fd_set getFdSetAvlbAndMd5Queues(int fdAvailableSlavesQueue,
                                int fdMd5Queue, int * maxFd) {
    fd_set fdSet;

    FD_ZERO(&fdSet);
    FD_SET(fdAvailableSlavesQueue, &fdSet);
    FD_SET(fdMd5Queue, &fdSet);

    *maxFd = (fdAvailableSlavesQueue > fdMd5Queue)?
              fdAvailableSlavesQueue : fdMd5Queue;

    return fdSet;
}


int getSlaveQuantity(int fileQuantity) {
    int slaveQuantity = 0;

    int numberOfProcessors = getNumberOfProcessors();

    if(fileQuantity <= numberOfProcessors) {
        slaveQuantity = fileQuantity;
    }
    else {
        if(fileQuantity >= numberOfProcessors * 2) {
            slaveQuantity = numberOfProcessors * 2;
        }
        else {
            slaveQuantity = fileQuantity / 2;
        }
    }

    return slaveQuantity;
}

int getNumberOfProcessors() {
    int fd[2];
    int numberOfProcessors = 0;
    if(pipe(fd) == ERROR_STATE) {
        error(MKPIPE_ERROR);
    }

    pid_t pid;
    if((pid = fork()) == ERROR_STATE) {
        error(FORK_ERROR);
    }

    if(pid == 0) {
        if(close(fd[0]) == ERROR_STATE) {
            error(CLOSE_ERROR);
        }

        if(dup2(fd[1], STDOUT_FILENO) == ERROR_STATE) {
            error(DUP_ERROR);
        }

        if(execl(NPROC_BIN_PATH, NPROC_BIN_NAME, NULL) == ERROR_STATE) {
            error(EXEC_ERROR(NRPC_BIN_PATH));
        }
    }
    else {
        if(close(fd[1]) == ERROR_STATE) {
            error(CLOSE_ERROR);
        }

        int status;
        waitpid(pid, &status, 0);

        char *numberOfProcessorsString;

        if((numberOfProcessorsString = getStringFromFd(fd[0], '\n')) == NULL) {
            error(READ_ERROR);
        }

        numberOfProcessors = stringToInt(numberOfProcessorsString);
        free(numberOfProcessorsString);
    }

    return numberOfProcessors;
}

int makeAvailableSlavesQueue(int slaveQuantity) {
    if(mkfifo(AVAILABLE_SLAVES_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
    }

    int fd;
    if((fd = open(AVAILABLE_SLAVES_QUEUE,
                  O_NONBLOCK | O_RDONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
    }

    if(fcntl(fd, F_SETPIPE_SZ, (MAX_PID_DIGITS + FORMAT_DIGITS) *
             slaveQuantity) == ERROR_STATE) {
           error(CHANGE_PIPE_SIZE_ERROR);
       }

    return fd;
}

void makeSlaves(int slaveQuantity, int fdAvailableSlavesQueue,
                  int fdMd5Queue) {
    pid_t pid;
    for(int i = 0; i < slaveQuantity; i++) {
        pid = fork();

        if(pid == ERROR_STATE) {
            error(FORK_SLAVE_ERROR);
        }

        if(pid == 0) {
            if(close(fdAvailableSlavesQueue) == ERROR_STATE) {
                error(CLOSE_ERROR);
            }

            if(close(fdMd5Queue) == ERROR_STATE) {
                error(CLOSE_ERROR);
            }

            if(execl(SLAVE_BIN_PATH, SLAVE_BIN_NAME, NULL) == ERROR_STATE) {
                error(EXEC_ERROR(SLAVE_BIN_PATH));
            }
        }
    }
}

int makeMd5ResultQueue() {
    if(mkfifo(MD5_RESULT_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR(MD5_RESULT_QUEUE));
    }

    int fd;
    if((fd = open(MD5_RESULT_QUEUE, O_NONBLOCK | O_RDONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(MD5_RESULT_QUEUE));
    }

    if(fcntl(fd, F_SETPIPE_SZ, 4 * (PATH_MAX + MD5_DIGITS + FORMAT_DIGITS)) ==
       ERROR_STATE) {
           error(CHANGE_PIPE_SIZE_ERROR);
       }

    return fd;
}
