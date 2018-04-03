#include <viewProcess.h>

int main(int argc, char const *argv[]) {
    if(argc != 1) {
      errorToStderr(INVALID_NUMBER_ARGS_ERROR);
  } else {
      int flag = TRUE;
      char *toPrint, aux;
      char *name;
      ShmBuffCDT sharedMemory = shmBuffAlreadyInit(argv[0]);
      while(flag) {
          if((name = getStringFromBuffer(sharedMemory)) == EOF) {
              flag = FALSE;
          } else {
              printf("%s\n", name);
              free(name);
          }
      }
      unmapSharedMemory(shmBuffPointer, argv[0]);
  }
    return 0;
}
