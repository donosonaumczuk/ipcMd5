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

        FILE *resultFile = fopen("hashMd5LastResult.txt","w");

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

        /* init semaphores here */

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
                    md5ResultBuffer = getStringFromFd(fdMd5Queue);
                }

                if(viewIsSet) {
                    if (writeInShmBuff(sharedMemory, md5ResultBuffer,
                        strlen(md5ResultBuffer) + 1) != OK_STATE) {
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
        fd_set fdSetBackup = getFdSetAvlbQueue(fdAvailableSlavesQueue);
        fd_set fdSet;

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
        close(fdMd5Queue);
        close(fdAvailableSlavesQueue);
        if(viewIsSet) {
            closeSharedMemory(sharedMemory, shmName);
        }
    }

    return 0;
}

char *getStringFromFd(int fd) {
    int i = 0, flag = TRUE, size = 0;
    signed char current;
    char *buffer = NULL;

    do {
        if(i % BLOCK == 0) {
            size =+ BLOCK;
            buffer = (char *) reAllocateMemory(buffer, size));
        }

        read(fd, &current, 1);

        if (current == EOF) {
            buffer = (char *)EOF;
            flag = FALSE;
        } else {
            if(current == 0) {
                flag = FALSE;
            }
            buffer[i++] = current;
        }

    } while(flag);

    return buffer;
}

fd_set getFdSetAvlbQueue(int fdAvailableSlavesQueue) {
    fd_set fdSet;

    FD_ZERO(&fdSet);
    FD_SET(fdAvailableSlavesQueue, &fdSet);

    return fdSet;
}

int readSlavePidString(int fdAvailableSlavesQueue, char *pidString) {
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

    return OK_STATE;
}

void sendToSlaveFileQueue(char *fifoName, char const *filePath) {
    int fd;
    if((fd = open(fifoName, O_WRONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(fifoName));
    }

    if(write(fd, filePath, strlen(filePath) + 1) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(filePath));
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

        char buffer[MAX_CORE_DIGITS] = {0};
        if(read(fd[0], buffer, MAX_CORE_DIGITS) == ERROR_STATE) {
            error(READ_ERROR);
        }

        numberOfProcessors = stringToInt(buffer);
    }

    return numberOfProcessors;
}

int makeAvailableSlavesQueue(int slaveQuantity) {
    if(mkfifo(AVAILABLE_SLAVES_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR);
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
        error(MKFIFO_ERROR);
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
