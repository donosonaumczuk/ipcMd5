#include <slave.h>

static void obtainHash(int fd, char *hash);
static void writeHashWithExpectedFormat(int fd, char *hash, char *filepath);

char *getPath(int fd) {
    return getStringFromFd(fd, 0);
}

void writeHashOnFd(int fd, char *filePath, sem_t *md5Sem, sem_t *pathsSem) {
    int status, fileDescriptors[2];
    pid_t pid;
    char hash[MD5_DIGITS + 1];
    if(pipe(fileDescriptors) == ERROR_STATE) {
        error(MKPIPE_ERROR);
    }
    pid = fork();
    if(pid == 0) {
        if(close(1) == ERROR_STATE) {
            error(CLOSE_ERROR);
        }
        if(dup(fileDescriptors[1]) == ERROR_STATE) {
            error(DUP_ERROR);
        }
        if(close(fileDescriptors[0]) == ERROR_STATE) {
            error(CLOSE_ERROR);
        }
        if(execl(MD5SUM,"md5sum", filePath,NULL) == ERROR_STATE) {
            error(EXEC_ERROR(MD5SUM));
        }
    }
    else if(pid == ERROR_STATE) {
        error(FORK_ERROR);
    }
    if(close(fileDescriptors[1]) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
    waitpid(pid, &status, 0);
    obtainHash(fileDescriptors[0],hash);
    printf("slave: enter sem_wait wirteOnHashFd\n"); //evans
    if(sem_wait(md5Sem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(MD5_SEMAPHORE));
    }
    printf("slave: exit sem_wait wirteOnHashFd\n"); //evans
    writeHashWithExpectedFormat(fd,hash,filePath);
    if(sem_post(md5Sem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(MD5_SEMAPHORE));
    }
    if(close(fileDescriptors[0]) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }

}

static void obtainHash(int fd, char *hash) {
    if(read(fd, hash, MD5_DIGITS) == ERROR_STATE) {
        printf("slave: obtaining hash error\n"); //evans
        error(READ_ERROR);
    }
    hash[MD5_DIGITS] = 0;
}

static void writeHashWithExpectedFormat(int fd, char *hash, char *filePath) {
    if(write(fd, filePath, strlen(filePath)) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(MD5_RESULT_QUEUE));
    }
    if(write(fd, ": ", strlen(": ")) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(MD5_RESULT_QUEUE));
    }
    if(write(fd, hash, MD5_DIGITS + 1) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(MD5_RESULT_QUEUE));
    }

}

void hashFilesOfGivenPaths(int number, int fdpaths, int fdmd5, sem_t *md5Sem, sem_t *pathsSem) {
    char *filePathToHash;
    while(number) {
<<<<<<< HEAD
        // waitForAnswer(fdpaths);//evans
         if(sem_wait(pathsSem) == ERROR_STATE) {
                error(SEMAPHORE_POST_ERROR(semaphorePathsName));
            }
=======
>>>>>>> e29cee4eae13dc9bd742cee80a43d48a7b3162a0
        filePathToHash = getPath(fdpaths);
         
        printf("slave: path get: %s\n", filePathToHash); //evans
        if(isValidFilePath(filePathToHash)) {
            writeHashOnFd(fdmd5,filePathToHash, md5Sem, pathsSem);
        }
        else {
            errorToStderr("Invalid file.\n");
        }
        number --;
        free(filePathToHash);
    }
}

int getNumberOfFilePaths(int fd) {
    char *buffer = getStringFromFd(fd, 0);
    return stringToInt(buffer);
}

void waitForAnswer(int fd) {
    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(fd, &readFds);
    if(select(fd + 1, &readFds, NULL, NULL, NULL) == ERROR_STATE) {
        error(SELECT_ERROR);
    }
}
