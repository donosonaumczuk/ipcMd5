#include "include/errors.h"

int error(char * errorMessage) {
    perror(errorMessage);
    exit(1);
}

int errorToStderr(char * errorMessage) {
    sprintf(errorMessage, "%s\n", STDERR);
    exit(1);
}
