#include <slaveProcess.h>

int main() {
    char semaphorePathsName[MAX_LONG_DIGITS + 2];
    sprintf(semaphorePathsName, "/%d", getpid());
    if(sem_unlink(semaphorePathsName) == ERROR_STATE) {
        error(SEMAPHORE_UNLINK_ERROR(semaphorePathsName));
    }
    sem_t *pathsSem = sem_open(semaphorePathsName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(pathsSem == SEM_FAILED) {
        error(OPEN_SEMAPHORE_ERROR(semaphorePathsName));
    }
    sem_t *requestSem = sem_open(AVAILABLE_SLAVES_SEMAPHORE, O_WRONLY);
    if(requestSem == SEM_FAILED) {
        error(OPEN_SEMAPHORE_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
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
        if(sem_wait(pathsSem) == ERROR_STATE) {
            error(SEMAPHORE_WAIT_ERROR(semaphorePathsName));
        }
        number = getNumberOfFilePaths(fdPaths);
        if(number) {
            hashFilesOfGivenPaths(number, fdPaths, fdMd5, md5Sem);
            if(sem_post(pathsSem) == ERROR_STATE) {
                error(SEMAPHORE_POST_ERROR(semaphorePathsName));
            }
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
    if(sem_close(pathsSem) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
    if(sem_unlink(semaphorePathsName) == ERROR_STATE) {
        error(SEMAPHORE_UNLINK_ERROR);
    }
    if(sem_close(requestSem) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
    if(sem_close(md5Sem) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
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
