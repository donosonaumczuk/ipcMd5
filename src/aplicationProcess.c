#include "aplicationProcess.h"

int main(int argc, char const *argv[]) {
    if(argc < 2) {
        errorToStderr(INVALID_NUMBER_ARGS_ERROR);
    }
    else {
        int fileQuantity = argc - 1;
        int slaveQuantity = getSlaveQuantity(fileQuantity);

        pid_t slavePids[slaveQuantity] = {0};
        makeSlaves(slavePids, slaveQuantity);

        int fdAvailableSlavesQueue = makeAvailableSlavesQueue();

        int fileToHashFds[slaveQuantity] = {0};
        makeFileToHashQueues(fileToHashFds, slavePids, slaveQuantity);
    }

    freeResources(); //may be not needed... but... to don't forget
    return 0;
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

        return stringToInt(buffer);
    }

    return ERROR_STATE;
}

int makeAvailableSlavesQueue() {
    if(mkfifo(AVAILABLE_SLAVES_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        return ERROR_STATE;
    }

    return open(AVAILABLE_SLAVES_QUEUE, O_RDONLY);
}

void makeFileToHashQueues(int *fileToHashQueues, int slaveQuantity,
    pid_t *slavePids) {
    char pidString[MAX_LONG_DIGITS] = {0};
    for(int i = 0; i < slaveQuantity; i++) {
        sprintf(pidString, "%d", slavePids[i]);
        if(mkfifo(pidString, S_IRUSR | S_IWUSR) == ERROR_STATE) {
            error(MKFIFO_ERROR);
        }
        if((fileToHashQueues[i] = open(fifoName, O_RDONLY)) == ERROR_STATE) {
            error(OPEN_FIFO_ERROR(fileToHashQueues[i]));
        }
    }
}

void makeSlaves(int *slavePids, int slaveQuantity) {
    int isChild = FALSE;
    int errorState = OK_STATE;

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
