#ifndef ERRORS_H

#include <stdlib.h>
#include <stdio.h>

#define ERRORS_H

#define ERROR_STATE -1
#define OK_STATE 0

#define INVALID_NUMBER_ARGS_ERROR "Invalid number of arguments"
#define ALLOC_MEMORY_ERROR "Error when trying to allocate dynamic memory"
#define EXEC_ERROR(filePath) "Error when trying to execute" #filePath "binary "\
                             "file through exec"
#define FORK_SLAVE_ERROR "Error when trying to fork Application Process "\
                          "to make a new Slave Process"
#define FORK_ERROR "Error when trying to fork"
#define DUP_ERROR "Error when trying to duplicate a file descriptor through dup"
#define MKFIFO_ERROR "Error when trying to make a fifo"
#define OPEN_FIFO_ERROR(fifoName) "Error when trying to open " #fifoName " fifo"
#define MKPIPE_ERROR "Error when trying to make pipe"
#define CLOSE_ERROR "Error when trying to close a file descriptor through close"
#define READ_ERROR "Error when trying to read a file descriptor through read"
#define SELECT_ERROR "Error when trying to monitor file descriptors through "\
                     "select"
#define ALLOCATE_MEM_ERROR "Error when trying to allocate memory"
#define OPEN_SHARE_MEMORY_ERROR "Error when trying to open a "\
                                "share memory object"
#define TRUNCATE_ERROR "Error when trying to extend share memory"
#define MAP_ERROR "Error trying to map share memory"
#define STAT_ERROR "Error trying to read share memory status"
#define KILL_ERROR "Error trying to send a message to a process"
#define SEMAPHORE_ERROR "Error trying to operate with semaphore"
#define UNMAP_ERROR "Error trying to unmap share memory"
#define UNLINK_SHARED_MEMORY_ERROR "Error trying to delete share memory object"
#define MISSING_HASH_ERROR "Error missing hash from a file"
#define WRITE_FIFO_ERROR(fifoName) "Error when trying to write on " #fifoName ""\
                              "fifo through write"
#define OPEN_SEMAPHORE_ERROR(semaphoreName) "Error trying to open semaphore " #semaphoreName 
#define SEMAPHORE_WAIT_ERROR(semaphoreName) "Error trying sem_wait on semaphore " #semaphoreName
#define SEMAPHORE_POST_ERROR(semaphoreName) "Error trying sem_post on semaphore " #semaphoreName
int error(char * errorMessage);
int errorToStderr(char * errorMessage);

#endif
