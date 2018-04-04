#include <viewProcess.h>

int main(int argc, char const *argv[]) {
    if(argc != 2) {
      errorToStderr(INVALID_NUMBER_ARGS_ERROR);
  } else {
      int flag = TRUE;
      char *name;
      int sharedMemoryFd = initSharedMemory(argv[1]);
      while(flag) {
          if(strcmp(name = getStringFromFd(sharedMemoryFd, 0), "") == EQUALS) {
              flag = FALSE;
          }
          else {
              printf("%s\n", name);
              free(name);
          }
      }
      unmapSharedMemory(sharedMemory, argv[0]);
  }
    return 0;
}
