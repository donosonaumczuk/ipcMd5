#ifndef ERRORS_H

#define ERRORS_H

#define ERROR_STATE -1
#define OK_STATE 0

#define INVALID_NUMBER_ARGS_ERROR "Invalid number of arguments"
#define ALLOC_MEMORY_ERROR "Error when trying to allocate dynamic memory"
#define EXEC_SLAVE_ERROR "Error when trying to execute a slave binary file through exec"
#define FORK_SLAVE_ERROR "Error when trying to fork Application Process to make a new Slave Process"
#define MKFIFO_ERROR "Error when trying to make a fifo"
#define OPEN_FIFO_ERROR "Error when trying to open a fifo"

int error(char * errorMessage);
int errorToStderr(char * errorMessage);

#endif
