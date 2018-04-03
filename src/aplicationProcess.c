#include <aplicationProcess.h>

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
                if(execl(VIEW_PROC_BIN_PATH, view_PROC_BIN_NAME, pidArgument,
                   NULL) == ERROR_STATE) {
                    error(EXEC_ERROR(view_PROC_BIN_PATH));
                }
            }

            nextFileIndex = 2;
            fileQuantity = argc - 2;
        }

        int slaveQuantity = getSlaveQuantity(fileQuantity);

        int fileLoad = getFileLoad(slaveQuantity, fileQuantity);

        int fdMd5Queue = makeMd5ResultQueue();

        int fdAvailableSlavesQueue = makeAvailableSlavesQueue();

        /* init semaphores */

        pid_t *slavePids = makeSlaves(slaveQuantity, fdAvailableSlavesQueue,
                                      fdMd5Queue);

        int remainingFiles = fileQuantity;


        int maxFd;
        fd_set fdSetBackup = getFdSet(fdAvailableSlavesQueue,
                                      fdMd5Queue, &maxFd);
        fd_set fdSet;

        while(remainingFiles > 0) {
            fdSet = fdSetBackup;
            monitorFds(maxFd, &fdSet);

            if(FD_ISSET(fdAvailableSlavesQueue, &fdSet)) {
                char pidString[MAX_PID_DIGITS + 1];
                while(readSlavePidString(fdAvailableSlavesQueue, pidString) !=
                      EMPTY) {
                    for(int i = 0; i < fileLoad && remainingFiles > 0; i++) {
                        sendNextFile(pidString, argv[nextFileIndex++]);
                        remainingFiles--;
                    }
                }
            }

            if(FD_ISSET(fdMd5Queue, &fdSet)) {
                readMd5Result(fdMd5Queue);
                /* write on file */
                if(viewIsSet) {
                    /* Write
                        on
                       ShmBuf */
                }
            }
        }

        /* here there is not remainingFiles, so i need to finish all slaves */

        // freeResources();
    }

    return 0;
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

void sendNextFile(char *fifoName, char const *filePath) {
    int fd;
    if((fd = open(fifoName, O_WRONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(fifoName));
    }

    if(write(fd, filePath, strlen(filePath)) == ERROR_STATE) {
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

fd_set getFdSet(int fdAvailableSlavesQueue, int fdMd5Queue, int * maxFd) {
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

int makeAvailableSlavesQueue() {
    if(mkfifo(AVAILABLE_SLAVES_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR);
    }

    int fd;
    if((fd = open(AVAILABLE_SLAVES_QUEUE,
                  O_NONBLOCK | O_RDONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
    }

    return fd;
}

/* D E P R E C A T E D

int *makeFileToHashQueues(pid_t *slavePids, int slaveQuantity) {
    int *fileToHashQueues = (int *) allocateMemory(slaveQuantity * sizeof(int));
    char pidString[MAX_LONG_DIGITS] = {0};
    for(int i = 0; i < slaveQuantity; i++) {
        sprintf(pidString, "%d", slavePids[i]);
        if(mkfifo(pidString, S_IRUSR | S_IWUSR) == ERROR_STATE) {
            error(MKFIFO_ERROR);declaration
        }
        if((fileToHashQueues[i] = open(pidString, O_RDONLY)) == ERROR_STATE) {
            error(OPEN_FIFO_ERROR(fileToHashQueues[i]));
        }
    }

    return fileToHashQueues;
} */

pid_t *makeSlaves(int slaveQuantity, int fdAvailableSlavesQueue,
                  int fdMd5Queue) {
    pid_t *slavePids = (pid_t *) allocateMemory(slaveQuantity * sizeof(pid_t));

    for(int i = 0; i < slaveQuantity; i++) {
        slavePids[i] = fork();

        if(slavePids[i] == ERROR_STATE) {
            error(FORK_SLAVE_ERROR);
        }

        if(slavePids[i] == 0) {
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

    return slavePids;
}

int makeMd5ResultQueue() {
    if(mkfifo(MD5_RESULT_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR);
    }

    int fd;
    if((fd = open(MD5_RESULT_QUEUE, O_NONBLOCK | O_RDONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(MD5_RESULT_QUEUE));
    }

    return fd;
}
