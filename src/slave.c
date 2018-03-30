#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "include/slave.h"

char *getPath(int fd) {
   char separator = ',';
   char *stringToReturn = NULL;
   int index = 0;
   int finished = FALSE;
   int readReturn;

   do{
      if((index % BLOCK) == 0) {
         stringToReturn = realloc(stringToReturn, (index + BLOCK) * sizeof(char));
      }
      readReturn = read(fd, (void *) (stringToReturn + (index * sizeof(char))), 1);
      if((readReturn <= 0) || (stringToReturn[index] == EOF) || (stringToReturn[index] == separator)) {
         stringToReturn[index] = 0;
         finished = TRUE;
      }
      index++;
   } while(!finished);
   return stringToReturn;
}

void writeHashOnFd(int fd, char *filePath) {
   int status;
   pid_t pid;
   pid = fork();
   if(pid == 0) {
      close(1);
      dup(fd);
      if(execl("/usr/bin/md5sum","md5sum", filePath,NULL) == -1) {
            perror("execl Failed.\n");
            exit(1);
     }
   }
   waitpid(pid, &status, 0);
}