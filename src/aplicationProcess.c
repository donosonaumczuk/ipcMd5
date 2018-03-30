#include "aplicationProcess.h"

int main(int argc, char const *argv[]) {
    if(argc == 1) {
        //ERROR, NO ARGUMENTS
    }
    else {
        int * slavePids = malloc(/* ? */);

        if(makeSlaves(/* slaveQty */, slavePids) == ERROR_STATE) {
            //ERROR, SLAVE CREATION FAILED
        }

        int fdSlavesQueue = makeAvailableSlavesQueue();
        if(fdSlavesQueue == ERROR_STATE) {
            //ERROR, FIFO CREATION FAILED
        }
    }

    return 0;
}



int makeAvailableSlavesQueue() {
    if(mkfifo(AVAILABLE_SLAVES_QUEUE, ) == ERROR_STATE) {
        return ERROR_STATE;
    }

    return open(AVAILABLE_SLAVES_QUEUE, O_RDONLY);
}

int makePathToHashQueues(int slaveQuantity, int * slavePids, int * pathToHashQueues) {
    char fifoName[MAX_LONG_DIGITS] = {0};
    for(int i = 0; i < slaveQuantity; i++) {
        sprintf(fifoName, "%d", slavePids[i]);
        mkfifo(fifoName, /* PERMISOS */); //check for error
        pathToHashQueues[i] = open(fifoName, O_RDONLY);
    }

    // return errorState;
}

int makeMd5ResultQueue() {
    mkfifo(MD5_RESULT_QUEUE, /* PERMISOS */); //check for error
    return open(MD5_RESULT_QUEUE, O_RDONLY); //check for error
}

int makeSlaves(int slaveQuantity, int * slavePids) {
    int isChild = FALSE;
    int errorState = OK_STATE;

    for(int i = 0; i < slaveQuantity && !isChild && errorState == OK_STATE; i++) {
        slavePids[i] = fork();

        if(slavePids[i] == ERROR_STATE) {
            errorState = ERROR_STATE;
        }

        else if(slavePids[i] == 0) {
            isChild = TRUE;
        }
    }

    if(isChild) {
        close();
        errorState = execl("./slave", NULL); //CHECK PATH
    }

    return errorState;
}
