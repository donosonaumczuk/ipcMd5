#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "slaveProcess.h"
#include "include/ipcMd5"
#include "include/errors.h"

int main() {
   char fifoPaths[MAX_LONG_DIGITS];
   sprinf(fifoPaths,"%d",getpid());
   int fdpaths, fdrequest, fdmd5;
   fdpaths = open(fifoPaths, O_RDONLY);
   if(fdpaths == ERROR_STATUS)
      error("Couldn't open fifo named: %s", fifoPaths);
   fdrequest = open(AVAILABLE_SLAVES_QUEUE, O_WRONLY);
   if(fdrequest == ERROR_STATUS) 
      error("Couldn't open fifo named: %s", AVAILABLE_SLAVES_QUEUE);
   fdmd5 = open(MD5_RESULT_QUEUE, O_WRONLY);
   if(fdmd5 == ERROR_STATUS) 
      error("Couldn't open fifo named: %s", MD5_RESULT_QUEUE);
   do {
      waitForAnswer(fifoPaths);
      number = getNumberOfFilePaths();
      hashFilesOfGivenPaths(number, fdpaths, fdmd5);
   } while(number);

   close(fdpaths);
   close(fdrequest);
   close(fdmd5);
   return 0;
}


