#include "include/view.h"

int main(int argc, char const *argv[]) {
    if(argc != 1) {
      errorToStderr(/*poner eso*/);
    }

    ShmBuffCDT shareMemory = shmBuffAlreadyInit(argv[0]);

    

    return 0;
}
