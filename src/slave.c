#include "include/slave.h"

static void obtainHash(int fd, char *hash);
static void writeHashWithExpectedFormat(int fd, char *hash, char *filepath);


char *getPath(int fd) {
    char separator = 0;
    char *stringToReturn = NULL;
    int index = 0;
    int finished = FALSE;
    int readReturn;

    do{
        if((index % BLOCK) == 0) {
            stringToReturn = (char *) reAllocateMemory(stringToReturn, (index + BLOCK) * sizeof(char));
        }
        readReturn = read(fd, (void *) (stringToReturn + (index * sizeof(char))), 1);
        if((readReturn <= 0) || (stringToReturn[index] == EOF) || (stringToReturn[index] == separator)) {
            stringToReturn[index] = 0;
            finished = TRUE;
        }
        index++;
    } while(!finished);
    return stringToReturn;
}

void writeHashOnFd(int fd, char *filePath, sem_t *md5Sem) {
    int status, fileDescriptors[2];
    pid_t pid;
    char hash[HASH_MD5_LENGTH + 1];
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
    if(sem_wait(md5Sem) == ERROR_STATE) {
        error(SEMAPHORE_WAIT_ERROR(MD5_SEMAPHORE));
    }
    writeHashWithExpectedFormat(fd,hash,filePath);
    if(sem_post(md5Sem) == ERROR_STATE) {
        error(SEMAPHORE_POST_ERROR(MD5_SEMAPHORE));
    }
    if(close(fileDescriptors[0]) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }

}

static void obtainHash(int fd, char *hash) {
    if(read(fd, hash, HASH_MD5_LENGTH) == ERROR_STATE) {
        error(READ_ERROR);
    }
    hash[HASH_MD5_LENGTH] = 0;
}

static void writeHashWithExpectedFormat(int fd, char *hash, char *filePath) {
    if(write(fd, filePath, strlen(filePath)) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(MD5_RESULT_QUEUE));
    }
    if(write(fd, ": ", strlen(": ")) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(MD5_RESULT_QUEUE));
    }
    if(write(fd, hash, HASH_MD5_LENGTH + 1) == ERROR_STATE) {
        error(WRITE_FIFO_ERROR(MD5_RESULT_QUEUE));
    }

}

void hashFilesOfGivenPaths(int number, int fdpaths, int fdmd5, sem_t *md5Sem) {
    char *filePathToHash;
    while(number) {
        filePathToHash = getPath(fdpaths);
        if(isValidFilePath(filePathToHash)) {
            writeHashOnFd(fdmd5,filePathToHash, md5Sem);
        }
        number --;
        free(filePathToHash);
    }
}

int getNumberOfFilePaths(int fd) {
    char quantity[MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY + 1];
    readNumber(fd, quantity, MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY);
    return stringToInt(quantity);
}

void readNumber(int fd, char *buffer, int count) {
    int i = 0, readquantity;
    char aux;
    do {
        readquantity = read(fd, &aux, 1);
        if(readquantity == ERROR_STATE) {
            error(READ_ERROR);
        }
        if(readquantity && isdigit(aux)) {
            buffer[i] = aux;
            i++;
        }

    } while(i < count && isdigit(aux) && readquantity);
    if(readquantity) {
        readquantity = read(fd, &aux, 1);
        if(readquantity == ERROR_STATE) {
            error(READ_ERROR);
        }
    }
    buffer[i] = 0;
}

void waitForAnswer(int fd) {
    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(fd, &readFds);
    if(select(fd + 1, &readFds, NULL, NULL, NULL) == ERROR_STATE) {
        error(SELECT_ERROR);
    }
}
