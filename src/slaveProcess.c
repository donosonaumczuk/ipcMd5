#include <slaveProcess.h>

int main() {
    sem_unlink("/requestSemaphore");
    sem_t *requestSem = sem_open("/requestSemaphore", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    sem_unlink("/md5Semaphore");
    sem_t *md5Sem = sem_open("/md5Semaphore", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);


    sem_t *requestSem = sem_open("/requestSemaphore", O_WRONLY);
    sem_t *md5Sem = sem_open("/md5Semaphore", O_WRONLY);

    
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
    createFilePathFifo(fifoPaths, fdRequest, requestSem);
    fdPaths = open(fifoPaths, O_RDONLY);
    if(fdPaths == ERROR_STATE) {
        error("");
    }
    do {
        waitForAnswer(fdPaths);
        number = getNumberOfFilePaths(fdPaths);
        if(number) {
            hashFilesOfGivenPaths(number, fdPaths, fdMd5);
            sem_wait(requestSem);
            if(write(fdRequest, fifoPaths, strlen(fifoPaths)) == ERROR_STATE) {
                error("");
            }
            sem_post(requestSem);
        }
    } while(number);

    close(fdPaths);
    close(fdRequest);
    close(fdMd5);
    return 0;
}

void createFilePathFifo(char *name, int fdRequest, sem_t *requestSem) {
    sprintf(name, "%d", getpid());
    if(mkfifo(name, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error("");
    }
    sem_wait(requestSem);
    if(write(fdRequest, name, strlen(name) + 1) == ERROR_STATE) {
        error("");
    }
    sem_post(requestSem);
}
