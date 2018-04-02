#include "include/tools.h"

int stringToInt(char *string) {
   int ret = 0,i;
   for(i = 0; string[i] && isdigit(string[i]); i++)
      ret = ret * 10 + (string[i] - '0');
   if(string[i])
      fprintf(stderr,"Couldn't convert string to int, the given string contains\
         characters diferent from digits.\n");
   return ret;
}

void * allocateMemory(size_t bytes) {
    void * address = malloc(bytes);

    if(address == NULL) {
        errorToStderr(ALLOCATE_MEM_ERROR);
    }

    return address;
}
