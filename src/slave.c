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
            stringToReturn = realloc(stringToReturn, (index + BLOCK) * sizeof(char));
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

void writeHashOnFd(int fd, char *filePath) {
    int status, fileDescriptors[2];
    pid_t pid;
    char hash[HASH_MD5_LENGTH + 1];
    if(pipe(fileDescriptors) == ERROR_STATE) {
        error("Pipe failed.\n");
    }
    pid = fork();
    if(pid == 0) {
        close(1);
        dup(fileDescriptors[1]);
        close(fileDescriptors[0]);
        if(execl("/usr/bin/md5sum","md5sum", filePath,NULL) == ERROR_STATE) {
            error("Couldn't execute md5sum.\n");
        }
    }
    else if(pid == ERROR_STATE) {
        error("Fork failed.\n");
    }
    close(fileDescriptors[1]);
    waitpid(pid, &status, 0);
    obtainHash(fileDescriptors[0],hash);
    writeHashWithExpectedFormat(fd,hash,filePath);
    close(fileDescriptors[0]);

}

static void obtainHash(int fd, char *hash) {
    read(fd, hash, HASH_MD5_LENGTH);
    hash[HASH_MD5_LENGTH] = 0;
}

static void writeHashWithExpectedFormat(int fd, char *hash, char *filePath) {
    write(fd, filePath, strlen(filePath));
    write(fd, ": ", strlen(": "));
    write(fd, hash, HASH_MD5_LENGTH + 1);

}

void hashFilesOfGivenPaths(int number, int fdpaths, int fdmd5) {
    char *filePathToHash;
    while(number) {
        filePathToHash = getPath(fdpaths);
        if(isValidFilePath) {
            writeHashOnFd(fdmd5,filePathToHash);
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
            error("");
        }
        if(readquantity && isdigit(aux)) {
            buffer[i] = aux;
            i++;
        }

    } while(i < count && isdigit(aux) && readquantity);
    if(readquantity) {
        readquantity = read(fd, &aux, 1);
        if(readquantity == ERROR_STATE) {
            error("");
        }
    }
    buffer[i] = 0;
}

void waitForAnswer(int fd) {
    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(fd, &readFds);
    if(select(fd + 1, &readFds, NULL, NULL, NULL) == ERROR_STATE) {
        error("");
    }
}
