#include <sharedMemory.h>

int createAndInitSharedMemory(char *shmName) {
    int fd;
    if((fd = shm_open(shmName, O_CREAT | O_RDWR, 0777)) //check permissions
       == ERROR_STATE) {
        error(OPEN_SHARED_MEMORY_ERROR);
    }

    if(ftruncate(fd, SHARED_MEMORY_SIZE) == ERROR_STATE) {
        error(TRUNCATE_ERROR);
    }

    return fd;
}


int initSharedMemory(char *shmName) {
    int fd;
    if((fd = shm_open(shmName, O_RDWR, 0777)) //check permissions
       == ERROR_STATE) {
        error(OPEN_SHARED_MEMORY_ERROR);
    }

    return fd;
}
