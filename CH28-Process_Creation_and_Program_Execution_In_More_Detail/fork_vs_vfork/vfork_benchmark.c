#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){
  
  for (int i = 0; i < 100000; i++){
    switch (vfork()) {
      case -1:
        perror("vfork");
        exit(EXIT_FAILURE);
      case 0:
        _exit(EXIT_SUCCESS);
      default:
        wait(NULL);
    }
  }
  exit(EXIT_SUCCESS);
}

