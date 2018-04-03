#include <tools.h>

int stringToInt(char *string) {
    int ret = 0,i;
    for(i = 0; string[i] && isdigit(string[i]); i++) {
        ret = ret * 10 + (string[i] - '0');
    }

    if(string[i]) {
      errorToStderr("Couldn't convert string to int, the given string contains\
         characters diferent from digits");
    }

    return ret;
}

void intToString(int integer, char *string) {
    sprintf(string, "%d", integer);
}

void * allocateMemory(size_t bytes) {
    void * address = malloc(bytes);

    if(address == NULL) {
        errorToStderr(ALLOCATE_MEM_ERROR);
    }

    return address;
}

void * reAllocateMemory(void *currentAddress, size_t bytes) {
    void * address = realloc(currentAddress, bytes);

    if(address == NULL) {
        errorToStderr(ALLOCATE_MEM_ERROR);
    }

    return address;
}

int isValidFilePath(char *filePath) {
    struct stat pathStat;
    stat(filePath, &pathStat);
    return S_ISREG(pathStat.st_mode);
}

char *getStringFromFd(int fd, char separator) {
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
