#include <viewProcess.h>

int main(int argc, char const *argv[]) {
    if(argc != 2) {
        errorToStderr(INVALID_NUMBER_ARGS_ERROR);
    } else {
     sem_t * emptySem = sem_open(EMPTY_SEMAPHORE, O_RDWR, S_IRUSR | S_IWUSR);

      if(emptySem == SEM_FAILED) {
          error(SEMAPHORE_OPEN_ERROR(EMPTY_SEMAPHORE));
      }

      sem_t * fullSem = sem_open(FULL_SEMAPHORE, O_RDWR, S_IRUSR | S_IWUSR);
      if (fullSem == SEM_FAILED) {
          error(SEMAPHORE_OPEN_ERROR(FULL_SEMAPHORE));
      }

      int flag = TRUE;
      char *name;
      ShmBuff_t sharedMemory = shmBuffAlreadyInit(argv[1]);
      while(flag) {
          if(strcmp(name = getStringFromBuffer(sharedMemory, emptySem, fullSem), "")
             == EQUALS) {
              flag = FALSE;
          }
          else {
              printf("%s\n", name);
              free(name);
          }
      }
      unmapSharedMemory(sharedMemory, argv[0]);

      if (sem_close(emptySem) == ERROR_STATE) {
          error(SEMAPHORE_CLOSE_ERROR(EMPTY_SEMAPHORE));
      }

      if (sem_close(fullSem) == ERROR_STATE) {
          error(SEMAPHORE_CLOSE_ERROR(FULL_SEMAPHORE));
      }
    }
    return 0;
}
