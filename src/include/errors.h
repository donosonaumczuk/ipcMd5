#ifndef ERRORS_H

#include <stdlib.h>
#include <stdio.h>

#define ERRORS_H

#define ERROR_STATE -1
#define OK_STATE 0

#define INVALID_NUMBER_ARGS_ERROR "Invalid number of arguments"
#define ALLOC_MEMORY_ERROR "Error when trying to allocate dynamic memory"
#define EXEC_ERROR(filePath) "Error when trying to execute" #filePath "binary"\
                             "file through exec"
#define FORK_SLAVE_ERROR "Error when trying to fork Application Process"\
                          " to make a new Slave Process"
#define FORK_ERROR "Error when trying to fork"
#define DUP_ERROR "Error when trying to duplicate a file descriptor through dup"
#define MKFIFO_ERROR "Error when trying to make a fifo"
#define OPEN_FIFO_ERROR(fifoName) "Error when trying to open " #fifoName " fifo"
#define MKPIPE_ERROR "Error when trying to make pipe"
#define CLOSE_ERROR "Error when trying to close a file descriptor through close"
#define READ_ERROR "Error when trying to read a file descriptor through read"
#define ALLOCATE_MEM_ERROR "Error when trying to allocate memory"

int error(char * errorMessage);
int errorToStderr(char * errorMessage);

#endif
