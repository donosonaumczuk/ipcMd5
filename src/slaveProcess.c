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

int main() {
   char fifoPaths[MAX_LONG_DIGITS];
   sprinf(fifoPaths,"%d",getpid());
   int fdpaths = open(fifoPaths, O_RDONLY);
   int fdrequest = open(AVAILABLE_SLAVES_QUEUE, O_WRONLY);
   int fdmd5 = open(MD5_RESULT_QUEUE, O_WRONLY);
   
   char * filePathToHash = getPath(fdpaths);

   writeHashOnFd(fdmd5,filePathToHash);
   

   // //ask for a new task
   close(fdpaths);
   close(fdrequest);
   close(fdmd5);
   return 0;
}

