#include "include/slaveProcess.h"

int main() {

    sem_t *requestSem = sem_open(REQUEST_SEMAPHORE, O_WRONLY);
    if(requestSem == SEM_FAILED) {
        error(OPEN_SEMAPHORE_ERROR(REQUEST_SEMAPHORE));
    }
    sem_t *md5Sem = sem_open(MD5_SEMAPHORE, O_WRONLY);
    if(md5Sem == SEM_FAILED) {
        error(OPEN_SEMAPHORE_ERROR(MD5_SEMAPHORE));
    }

    char fifoPaths[MAX_LONG_DIGITS];
    int fdPaths, fdRequest, fdMd5, number;
    fdRequest = open(AVAILABLE_SLAVES_QUEUE, O_WRONLY);
    if(fdRequest == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
    }
    fdMd5 = open(MD5_RESULT_QUEUE, O_WRONLY);
    if(fdMd5 == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(MD5_RESULT_QUEUE));
    }

    createFilePathFifo(fifoPaths, fdRequest, requestSem);
    fdPaths = open(fifoPaths, O_RDONLY);
    if(fdPaths == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(fifoPaths));
    }

    if(fcntl(fdPaths, F_SETPIPE_SZ, GREATEST_FILE_LOAD * (PATH_MAX + 1)) < 0) {
        error(CHANGE_PIPE_SIZE_ERROR);
    }

    do {
        waitForAnswer(fdPaths);
        number = getNumberOfFilePaths(fdPaths);
        if(number) {
            hashFilesOfGivenPaths(number, fdPaths, fdMd5, md5Sem);
            if(sem_wait(requestSem) == ERROR_STATE) {
                error(SEMAPHORE_WAIT_ERROR(REQUEST_SEMAPHORE));
            }
            if(write(fdRequest, fifoPaths, strlen(fifoPaths)) == ERROR_STATE) {
                error(WRITE_FIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
            }
            if(sem_post(requestSem) == ERROR_STATE) {
                error(SEMAPHORE_POST_ERROR(REQUEST_SEMAPHORE));
            }
        }
    } while(number);

    if(close(fdPaths) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
    if(close(fdRequest) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
    if(close(fdMd5) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
    return 0;
}

void createFilePathFifo(char *name, int fdRequest, sem_t *requestSem) {
    sprintf(name, "%d", getpid());
    if(mkfifo(name, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR(name));
    }
    if(sem_wait(requestSem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(REQUEST_SEMAPHORE));
    }
    if(write(fdRequest, name, strlen(name) + 1) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
    }
    if(sem_post(requestSem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(REQUEST_SEMAPHORE));
    }
}
