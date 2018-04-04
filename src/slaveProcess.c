#include <slaveProcess.h>

int main() {
    char semaphorePathsName[MAX_LONG_DIGITS + 2];
    sprintf(semaphorePathsName, "/%d", getpid());
    if(sem_unlink(semaphorePathsName) == ERROR_STATE) {
        if(errno != ENOENT) {
            error(SEMAPHORE_UNLINK_ERROR(semaphorePathsName));
        }
    }
    sem_t *pathsSem = sem_open(semaphorePathsName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    if(pathsSem == SEM_FAILED) {
        error(SEMAPHORE_OPEN_ERROR(semaphorePathsName));
    }
    sem_t *availableSlavesSem = sem_open(AVAILABLE_SLAVES_SEMAPHORE, O_WRONLY);
    if(availableSlavesSem == SEM_FAILED) {
        error(SEMAPHORE_OPEN_ERROR(AVAILABLE_SLAVES_SEMAPHORE));
    }
    sem_t *md5Sem = sem_open(MD5_SEMAPHORE, O_WRONLY);
    if(md5Sem == SEM_FAILED) {
        error(SEMAPHORE_OPEN_ERROR(MD5_SEMAPHORE));
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

    fdPaths = createFilePathFifo(fifoPaths, fdRequest, availableSlavesSem);
    printf("slave: %d\n", getpid()); //evans
        
    do {
        printf("slave: entering waitForAnswer");
        waitForAnswer(fdPaths);
        printf("slave: exit waitForAnswer\n"); //evans
        if(sem_wait(pathsSem) == ERROR_STATE) {
            error(SEMAPHORE_WAIT_ERROR(semaphorePathsName));
        }
        number = getNumberOfFilePaths(fdPaths);
         if(sem_post(pathsSem) == ERROR_STATE) {
                error(SEMAPHORE_POST_ERROR(semaphorePathsName));
            }
        printf("slave: number readed: %d\n", number);//evans
        if(number) {
            hashFilesOfGivenPaths(number, fdPaths, fdMd5, md5Sem, pathsSem);
            printf("slave: post hashFilesOfGivenPaths %d\n", number); //evans
            printf("before entering availableSlavesSem wait.\n");
            if(sem_wait(availableSlavesSem) == ERROR_STATE) {
                error(SEMAPHORE_WAIT_ERROR(REQUEST_SEMAPHORE));
            }
            printf("slave: Writting pid"); //evans
            if(write(fdRequest, fifoPaths, strlen(fifoPaths) + 1) == ERROR_STATE) {
                error(WRITE_FIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
            }
            if(sem_post(availableSlavesSem) == ERROR_STATE) {
                error(SEMAPHORE_POST_ERROR(REQUEST_SEMAPHORE));
            }
        }
    } while(number);
    // if(sem_post(pathsSem) == ERROR_STATE) {
    //     error(SEMAPHORE_POST_ERROR(semaphorePathsName));
    // }
    if(sem_close(pathsSem) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
    if(sem_unlink(semaphorePathsName) == ERROR_STATE) {
        if(errno != ENOENT) {
            error(SEMAPHORE_UNLINK_ERROR(semaphorePathsName));
        }
    }
    if(sem_close(availableSlavesSem) == ERROR_STATE) {
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
    printf("slave: ends\n");
    return 0;
}

int createFilePathFifo(char *name, int fdRequest, sem_t *availableSlavesSem) {
    int fd;
    sprintf(name, "%d", getpid());
    if(mkfifo(name, S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR(name));
    }
    if((fd = open(name, O_NONBLOCK | O_RDONLY)) == ERROR_STATE) {
        error(OPEN_FIFO_ERROR(name));
    }
    if(fcntl(fd, F_SETPIPE_SZ, GREATEST_FILE_LOAD * (PATH_MAX + 1)) < 0) {
        error(CHANGE_PIPE_SIZE_ERROR);
    }
    if(sem_wait(availableSlavesSem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(REQUEST_SEMAPHORE));
    }
    if(write(fdRequest, name, strlen(name) + 1) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(AVAILABLE_SLAVES_QUEUE));
    }
    if(sem_post(availableSlavesSem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(REQUEST_SEMAPHORE));
    }
    return fd;
}
