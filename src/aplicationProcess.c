#include "include/aplicationProcess.h"

int main(int argc, char const *argv[]) {
    if(argc < 2) {
        errorToStderr(INVALID_NUMBER_ARGS_ERROR);
    }
    else {
        int fileQuantity = argc - 1;

        int slaveQuantity = getSlaveQuantity(fileQuantity);

        pid_t *slavePids = makeSlaves(slaveQuantity);

        int fdAvailableSlavesQueue = makeAvailableSlavesQueue();

        int *fileToHashFds = makeFileToHashQueues(slavePids, slaveQuantity);

        // freeResources();
    }

    return 0;
}

void * allocateMemory(size_t bytes) {
    void * address = malloc(bytes);

    if(address == NULL) {
        errorToStderr(ALLOCATE_MEM_ERROR);
    }

    return address;
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
    if((fd = open(AVAILABLE_SLAVES_QUEUE, O_RDONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
    }

    return fd;
}

int *makeFileToHashQueues(pid_t *slavePids, int slaveQuantity) {
    int *fileToHashQueues = (int *) allocateMemory(slaveQuantity * sizeof(int));
    char pidString[MAX_LONG_DIGITS] = {0};
    for(int i = 0; i < slaveQuantity; i++) {
        sprintf(pidString, "%d", slavePids[i]);
        if(mkfifo(pidString, S_IRUSR | S_IWUSR) == ERROR_STATE) {
            error(MKFIFO_ERROR);
        }
        if((fileToHashQueues[i] = open(pidString, O_RDONLY)) == ERROR_STATE) {
            error(OPEN_FIFO_ERROR(fileToHashQueues[i]));
        }
    }

    return fileToHashQueues;
}

pid_t *makeSlaves(int slaveQuantity) {
    int isChild = FALSE;
    int errorState = OK_STATE;

    pid_t *slavePids = (pid_t *) allocateMemory(slaveQuantity * sizeof(pid_t));

    for(int i = 0; i < slaveQuantity && !isChild
        && errorState == OK_STATE; i++) {

        slavePids[i] = fork();

        if(slavePids[i] == ERROR_STATE) {
            errorState = ERROR_STATE;
        }

        if(slavePids[i] == 0) {
            isChild = TRUE;
        }
    }

    if(errorState != OK_STATE) {
        error(FORK_SLAVE_ERROR);
    }

    if(isChild) {
        if(execl(SLAVE_BIN_PATH, SLAVE_BIN_NAME, NULL) == ERROR_STATE) {
            error(EXEC_ERROR(SLAVE_BIN_PATH));
        }
    }

    return slavePids;
}

int makeMd5ResultQueue() {
    if(mkfifo(MD5_RESULT_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR);
    }

    int fdOpen;
    if((fdOpen = open(MD5_RESULT_QUEUE, O_RDONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(MD5_RESULT_QUEUE));
    }

    return fdOpen;
}
