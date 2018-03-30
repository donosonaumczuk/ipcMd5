#ifndef ERRORS_H

#define ERRORS_H

#define ERROR_STATE -1
#define OK_STATE 0

#define INVALID_NUMBER_ARGS_ERROR "Invalid number of arguments"

int error(char * errorMessage);
int errorToStderr(char * errorMessage);

#endif
