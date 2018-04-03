#include <viewProcess.h>

int main(int argc, char const *argv[]) {
    if(argc != 1) {
      errorToStderr(INVALID_NUMBER_ARGS_ERROR);
  } else {
      int flag = TRUE;
      char *toPrint, aux;
      char *name, hash;
      ShmBuff_t sharedMemory = shmBuffAlreadyInit(argv[0]);
      while(flag) {
          if((name = getStringFromBuffer(sharedMemory)) == EOF) {
              flag = FALSE;
          } else if ((hash = getStringFromBuffer(sharedMemory)) == EOF)) {
              free(name);
              flag = FALSE;
              errorToStderr(MISSING_HASH_ERROR);
          } else {
              printf("<%s>:<%s>\n", name, hash);
              free(name);
              free(hash);
          }
      }
      unmapSharedMemory(shmBuffPointer, argv[0]);
  }
    return 0;
}
