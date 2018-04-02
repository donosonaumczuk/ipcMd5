#include "include/slaveProcess.h"

int main() {
    char fifoPaths[MAX_LONG_DIGITS];
    int fdPaths, fdRequest, fdMd5, number;
    fdRequest = open(AVAILABLE_SLAVES_QUEUE, O_WRONLY);
    if(fdRequest == ERROR_STATE) {
        error("");
    }
    fdMd5 = open(MD5_RESULT_QUEUE, O_WRONLY);
    if(fdMd5 == ERROR_STATE) {
        error("");
    }
    createFilePathFifo(fifoPaths, fdRequest);
    fdPaths = open(fifoPaths, O_RDONLY);
    if(fdPaths == ERROR_STATE) {
        error("");
    }
    do {
        waitForAnswer(fdPaths);
        number = getNumberOfFilePaths(fdPaths);
        if(number) {
            hashFilesOfGivenPaths(number, fdPaths, fdMd5);
            if(write(fdRequest, fifoPaths, strlen(fifoPaths)) == ERROR_STATE) {
                error("");
            }
        }
    } while(number);

    close(fdPaths);
    close(fdRequest);
    close(fdMd5);
    return 0;
}

void createFilePathFifo(char *name, int fdRequest) {
    sprintf(name, "%d", getpid());
    if(mkfifo(name, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error("");
    }
    if(write(fdRequest, name, strlen(name) + 1) == ERROR_STATE) {
        error("");
    }
}
