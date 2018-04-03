#include "include/slaveTest.h"

/* The suite initialization function.
* Opens the temporary file used by the tests.
* Returns zero on success, non-zero otherwise.
*/
int initSlaveSuite() {
    return 0;
}




/* The suite cleanup function.
* Closes the temporary file used by the tests.
* Returns zero on success, non-zero otherwise.
*/
int cleanSlaveSuite() {
    return 0;
}


void testOfReadingTheQuantityOfFilePathsToHash() {
    int fd, actualQuantity = 9, readQuantity;
    givenAFileDescriptorToReadAQuantity(&fd);
    //A quantity is valid if its quantity of digits is less or
    //equal to MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY
    givenAValidQuantityToReadOnAFile(fd, actualQuantity);

    readQuantity = whenTheNumberIsRead(fd);

    thenTheQuantityiesMustBeTheSame(actualQuantity, readQuantity);
}

void givenAFileDescriptorToReadAQuantity(int *fd) {
    int ret = open("testOfReadingTheQuantityOfFilePathsToHash.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(ret == ERROR_STATE) {
        error(OPEN_FILE_ERROR("testOfReadingTheQuantityOfFilePathsToHash.txt"));
    }
    *fd = ret;
}

void givenAValidQuantityToReadOnAFile(int fd, int actualQuantity) {
    char buffer[MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY + 1];
    sprintf(buffer, "%d", actualQuantity);
    if(write(fd, buffer, MAX_QUANTITY_OF_DIGITS_OF_FILE_PATHS_QUANTITY) == ERROR_STATE) {
        error(WRITE_FILE_ERROR("testOfReadingTheQuantityOfFilePathsToHash.txt"));
    }
}

int whenTheNumberIsRead(int fd) {
    lseek(fd, 0, SEEK_SET);
    return getNumberOfFilePaths(fd);
}

void thenTheQuantityiesMustBeTheSame(int actualQuantity, int readQuantity) {
    CU_ASSERT(actualQuantity == readQuantity);
}

void testWriteHashOnAFIFO() {
    char *filePath, *fifoName;

    fifoName = givenAFifo();
    filePath = givenThePathOfAFileToHash();

    whenASlaveWritesAHashOnAFifo(fifoName, filePath);

    thenHashIsWrittenOnAFifo(fifoName);
}

char *givenAFifo() {
    if(mkfifo("fifoTest", S_IRUSR | S_IWUSR) == ERROR_STATE) {
        error(MKFIFO_ERROR);
    }
    return "fifoTest";
}

char *givenThePathOfAFileToHash() {
    return FILE_PATH_TO_HASH;
}

void whenASlaveWritesAHashOnAFifo(char *fifoName, char *filePathToHash) {
    int fifoFd = open(fifoName, O_RDWR);
    writeHashOnFd(fifoFd, filePathToHash);
}

void thenHashIsWrittenOnAFifo(char *fifoName) {
    int length = HASH_MD5_LENGTH + 2 + FILE_PATH_TO_HASH_LENGTH;
    char hash[length + 1];
    int fifoFd = open(fifoName, O_RDWR);
    if(read(fifoFd,hash, length) == ERROR_STATE) {
        error(READ_ERROR);
    }
    hash[length] = 0;
    CU_ASSERT(strcmp(hash, HASH) == 0);

}

void testReadAFilePath() {
    int fd;
    char *path;
    fd = givenAFileDescriptorWithSomethingWritten();

    path = whenAFilePathIsReadFromFileDescriptor(fd);

    thenFilesMustBeTheSame(path);
}

int givenAFileDescriptorWithSomethingWritten() {
    int fd = open("testReadAFilePath.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd == ERROR_STATE) {
        error(OPEN_ERROR("testReadAFilePath.txt"));
    }
    if(write(fd, FILE_PATH_TO_READ, FILE_PATH_TO_READ_LENGTH) == ERROR_STATE) {
        error(WRITE_FILE_ERROR("testReadAFilePath.txt"));
    }
    lseek(fd, 0, SEEK_SET);
    return fd;
}


char *whenAFilePathIsReadFromFileDescriptor(int fd){
    char *path = getPath(fd);
    if(close(fd) == ERROR_STATE) {
        error(CLOSE_ERROR);
    }
    return path;
}

void thenFilesMustBeTheSame(char *path) {

    CU_ASSERT(strcmp(path, "./test/PathName/ToRead.txt") == 0);


}
