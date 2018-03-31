#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "include/slave.h"
#include "include/errors.h"

static void obtainHash(int fd, char *hash);
static void writeHashWithExpectedFormat(int fd, char *hash, char *filepath);


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
   int status, fileDescriptors[2];
   pid_t pid;
   char hash[HASH_MD5_LENGTH + 1];
   if(pipe(fileDescriptors) == ERROR_STATE)
      error("Pipe failed.\n");
   pid = fork();
   if(pid == 0) {
      close(1);
      dup(fileDescriptors[1]);
      close(fileDescriptors[0]);
      if(execl("/usr/bin/md5sum","md5sum", filePath,NULL) == ERROR_STATE)
         error("Couldn't execute md5sum.\n");
   }
   else if(pid == ERROR_STATE)
      error("Fork failed.\n");
   close(fileDescriptors[1]);
   waitpid(pid, &status, 0);
   obtainHash(fileDescriptors[0],hash);
   writeHashWithExpectedFormat(fd,hash,filePath);
   close(fileDescriptors[0]);
   
}

static void obtainHash(int fd, char *hash) {
   read(fd, hash, HASH_MD5_LENGTH);
   hash[HASH_MD5_LENGTH] = 0;
}

static void writeHashWithExpectedFormat(int fd, char *hash, char *filePath) {
   write(fd, filePath, strlen(filePath));
   write(fd, ": ", 2);
   write(fd, hash, HASH_MD5_LENGTH + 1);

}