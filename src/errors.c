#include "include/errors.h"

/* Must be used only when errno was set. Prints errorMessage
   to STDERR via perror function which will print a message based
   on errno set value */
int error(char * errorMessage) {
    perror(errorMessage);
    exit(1);
}

/* Prints errorMessage to STDERR */
int errorToStderr(char * errorMessage) {
    fprintf(stderr, "%s\n", errorMessage);
    exit(1);
}
