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

int main() {
   // int fdmd5 = open(MD5_RESULT_QUEUE, O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
   // int fdpaths = open(AVAILABLE_SLAVES_QUEUE, O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
   
   // char * filePathToHash = getPath(fdpaths);

   // writeHashOnFd(fdmd5,filePathToHash);
   

   // //ask for a new task
   // close(fd);
   return 0;
}

