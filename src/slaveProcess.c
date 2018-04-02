#include "include/slaveProcess.h"

int main() {
<<<<<<< HEAD
   char fifoPaths[MAX_LONG_DIGITS];
   int fdpaths, fdrequest, fdmd5;
   fdrequest = open(AVAILABLE_SLAVES_QUEUE, O_WRONLY);
   if(fdrequest == ERROR_STATUS)
      error("Couldn't open fifo named: %s", AVAILABLE_SLAVES_QUEUE);
   createFilePathFifo(fifoPaths, fdrequest);
   fdpaths = open(fifoPaths, O_RDONLY);
   if(fdpaths == ERROR_STATUS)
      error("Couldn't open fifo named: %s", fifoPaths);
   fdmd5 = open(MD5_RESULT_QUEUE, O_WRONLY);
   if(fdmd5 == ERROR_STATUS)
      error("Couldn't open fifo named: %s", MD5_RESULT_QUEUE);
   do {
      waitForAnswer(fifoPaths);
      number = getNumberOfFilePaths();
      if(number) {
         hashFilesOfGivenPaths(number, fdpaths, fdmd5);
         if(write(fdrequest, fifoPaths) == ERROR_STATUS)
            error("");
      }
   } while(number);
=======
    char fifoPaths[MAX_LONG_DIGITS];
    int fdpaths, fdrequest, fdmd5;
    fdrequest = open(AVAILABLE_SLAVES_QUEUE, O_WRONLY);
    if(fdrequest == ERROR_STATUS) {
        error("Couldn't open fifo named: %s", AVAILABLE_SLAVES_QUEUE);
    }
    createFilePathFifo(fifoPaths, fdrequest);
    fdpaths = open(fifoPaths, O_RDONLY);
    if(fdpaths == ERROR_STATUS) {
        error("Couldn't open fifo named: %s", fifoPaths);
    }
    fdmd5 = open(MD5_RESULT_QUEUE, O_WRONLY);
    if(fdmd5 == ERROR_STATUS) {
        error("Couldn't open fifo named: %s", MD5_RESULT_QUEUE);
    }
    do {
        waitForAnswer(fifoPaths);
        number = getNumberOfFilePaths();
        if(number) {
            hashFilesOfGivenPaths(number, fdpaths, fdmd5);
            if(write(fdrequest, fifoPaths) == ERROR_STATUS) {
                error("");
            }
        }
    } while(number);
>>>>>>> b2b936e94dab499ec517c836d68ed3323d33fad1

    close(fdpaths);
    close(fdrequest);
    close(fdmd5);
    return 0;
}

void createFilePathFifo(char *name, int fdrequest) {
    sprinf(name, "%d", getpid());
    if(mkfifo(name, S_IRUSR | S_IWUSR) == ERROR_STATUS) {
        error("Couldn't make fifo named: %s\n", name);
    }
    if(write(fdrequest, name, strlen(name) + 1) == ERROR_STATUS) {
        error("");
    }
}
