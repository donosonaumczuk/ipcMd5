#include "aplicationProcess.h"

int main(int argc, char const *argv[]) {
    if(argc == 1) {
        errorToStderr(INVALID_NUMBER_ARGS_ERROR);
    }
    else {
        slaveQuantity = /* some criteria */;
        pid_t slavePids[slaveQuantity] = {0};
        makeSlaves(slavePids, slaveQuantity);
        int fdAvailableSlavesQueue = makeAvailableSlavesQueue();
        int fileToHashFds[slaveQuantity] = {0};
        makeFileToHashQueues(fileToHashFds, slavePids, slaveQuantity);
    }

    freeResources(); //may be not needed... but... to don't forget
    return 0;
}

int makeAvailableSlavesQueue() {
    if(mkfifo(AVAILABLE_SLAVES_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        return ERROR_STATE;
    }

    return open(AVAILABLE_SLAVES_QUEUE, O_RDONLY);
}

void makeFileToHashQueues(int *fileToHashQueues, int slaveQuantity, pid_t *slavePids) {
    char pidString[MAX_LONG_DIGITS] = {0};
    for(int i = 0; i < slaveQuantity; i++) {
        sprintf(pidString, "%d", slavePids[i]);
        if(mkfifo(pidString, S_IRUSR | S_IWUSR) == ERROR_STATE) {
            error(MKFIFO_ERROR);
        }
        if((fileToHashQueues[i] = open(fifoName, O_RDONLY)) == ERROR_STATE) {
            error(OPEN_FIFO_ERROR);
        }
    }
}

int makeMd5ResultQueue() {
    if(mkfifo(MD5_RESULT_QUEUE, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR);
    }

    int fdOpen;
    if((fdOpen = open(MD5_RESULT_QUEUE, O_RDONLY) {
        error(OPEN_FIFO_ERROR);
    }

    return fdOpen;
}

void makeSlaves(int *slavePids, int slaveQuantity) {
    int isChild = FALSE;
    int errorState = OK_STATE;

    for(int i = 0; i < slaveQuantity && !isChild && errorState == OK_STATE; i++) {
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
        if(execl(SLAVE_BIN_PATH, NULL) == ERROR_STATE) {
            error(EXEC_SLAVE_ERROR);
        }
    }
}
