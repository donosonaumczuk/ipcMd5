#include <viewProcess.h>

int main(int argc, char const *argv[]) {
    if(argc != 2) {
      errorToStderr(INVALID_NUMBER_ARGS_ERROR);
  } else {
      int flag = TRUE;
      char *name;
      ShmBuff_t sharedMemory = shmBuffAlreadyInit(argv[0]);
      while(flag) {
          if((name = getStringFromBuffer(sharedMemory)) == NULL) {
              flag = FALSE;
          } else {
              printf("%s\n", name);
              free(name);
          }
      }
      unmapSharedMemory(sharedMemory, argv[0]);
  }
    return 0;
}
