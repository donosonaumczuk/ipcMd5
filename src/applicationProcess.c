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
        pid_t viewPid;
        char applicationPidString[MAX_PID_DIGITS];

        if(strcmp(argv[1], VIEW_PROC_FLAG) == EQUALS) {
            viewIsSet = TRUE;

            if((viewPid = fork()) == ERROR_STATE) {
                error(FORK_ERROR);
            }

            if(viewPid == 0) {
                if(kill(getpid(), SIGSTOP) == ERROR_STATE) {
                    error(KILL_ERROR);
                }

                intToString(applicationPid, applicationPidString);
                if(execl(VIEW_PROC_BIN_PATH, VIEW_PROC_BIN_NAME, applicationPidString,
                   NULL) == ERROR_STATE) {
                    error(EXEC_ERROR(VIEW_PROC_BIN_PATH));
                }
            }

            nextFileIndex = 2;
            fileQuantity = argc - 2;
        }

        FILE *resultFile = fopen(MD5_RESULT_FILE, WRITE_PERMISSION);

        ShmBuff_t sharedMemory;
        if(viewIsSet) {
            intToString(applicationPid, applicationPidString);

            sharedMemory = shmBuffInit(applicationPidString);

            if(kill(viewPid, SIGCONT) == ERROR_STATE) {
                error(KILL_ERROR);
            }

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

        int pendingWrite = FALSE;
        char *md5ResultBuffer;

        int resultsWrited = 0;
        int resultsRead = 0;
        char pidString[MAX_PID_DIGITS + 1];
        int filesToSentQuantity = fileLoad;
        char filesToSentQuantityString[GREATEST_LOAD_DIGITS + 1];

        while(remainingFiles > 0 || resultsWrited < fileQuantity) {
            fdSet = fdSetBackup;

            if(resultsRead < fileQuantity) {
                monitorFds(maxFd, &fdSet);
            }

            if(FD_ISSET(fdAvailableSlavesQueue, &fdSet) && remainingFiles > 0) {
                while(remainingFiles > 0 && (readSlavePidString(fdAvailableSlavesQueue, pidString,
                                         availableSlavesSem) != EMPTY)) {
                    if(fileLoad > remainingFiles) {
                        filesToSentQuantity = remainingFiles;
                    }

                    int fd;
                    sem_t *fileQueueSem = waitSlaveFileQueue(pidString, &fd);

                    intToString(filesToSentQuantity, filesToSentQuantityString);
                    writeToFd(filesToSentQuantityString, fd);

                    for(int i = 0; i < filesToSentQuantity; i++) {
                        writeToFd(argv[nextFileIndex++], fd);
                        remainingFiles--;
                    }

                    postSlaveFileQueue(fileQueueSem, fd);
                }
            }

            if(FD_ISSET(fdMd5Queue, &fdSet) || pendingWrite) {
                if(!pendingWrite) {
                    md5ResultBuffer = getMd5QueueResult(fdMd5Queue,
                                                        md5QueueSem);
                    resultsRead++;
                }

                if(viewIsSet) {
                    if (writeInShmBuff(sharedMemory,
                                      ( char *) md5ResultBuffer,
                                       strlen(md5ResultBuffer) + 1) !=
                                       OK_STATE) {
                        pendingWrite = TRUE;
                    }
                    else {
                        pendingWrite = FALSE;
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
            monitorFds(maxFd, &fdSet);
            readSlavePidString(fdAvailableSlavesQueue, pidString,
                               availableSlavesSem);
            intToString(0, filesToSentQuantityString);

            int fd;
            sem_t *fileQueueSem = waitSlaveFileQueue(pidString, &fd);

            writeToFd(filesToSentQuantityString, fd);

            postSlaveFileQueue(fileQueueSem, fd);

            finishRequestedSlaves++;
        }

        int status;
        int finishedProcesses = 0;
        int childrenProcesses = slaveQuantity;

        if(viewIsSet) {
            childrenProcesses++;
            closeSharedMemory(sharedMemory, applicationPidString);
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
    }

    return 0;
}

sem_t *waitSlaveFileQueue(char *slavePidString, int *fd) {
    if((*fd = open(slavePidString, O_NONBLOCK | O_WRONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(slavePidString));
    }

    char semName[MAX_PID_DIGITS + 2] = {0};

    strcat(semName, "/");
    strcat(semName, slavePidString);

    sem_t *slaveFileQueueSem = sem_open(semName, O_WRONLY);
    if(slaveFileQueueSem == SEM_FAILED) {
        error(SEMAPHORE_OPEN_ERROR(semName));
    }

    if(sem_wait(slaveFileQueueSem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(semName));
    }

    return slaveFileQueueSem;
}

void postSlaveFileQueue(sem_t *slaveFileQueueSem, int fd) {
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
        if(errno != ENOENT) {
            error(SEMAPHORE_UNLINK_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
        }
    }

    if(sem_unlink(MD5_SEMAPHORE) == ERROR_STATE) {
        if(errno != ENOENT) {
            error(SEMAPHORE_UNLINK_ERROR(MD5_SEMAPHORE));
        }
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

    *md5QueueSem = sem_open(MD5_SEMAPHORE, O_CREAT | O_RDWR,
                               S_IRUSR | S_IWUSR, ONE_RESOURCE);

    if(*md5QueueSem == SEM_FAILED) {
        error(SEMAPHORE_OPEN_ERROR(MD5_SEMAPHORE));
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
            if(sem_post(availableSlavesSem) == ERROR_STATE) {
                error(SEMAPHORE_POST_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
            }
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

void writeToFd(char const *string, int fd) {
    if(write(fd, string, strlen(string) + 1) == ERROR_STATE) {
        error(WRITE_FILE_ERROR("")); //check later
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
