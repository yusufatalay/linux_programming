#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  switch (vfork()) {
  case -1:
    perror("vfork");
    exit(EXIT_FAILURE);
  case 0:
    printf("Child is about to close its stdout descriptor\n");
    close(STDOUT_FILENO);
    _exit(EXIT_SUCCESS);
  default:
    wait(NULL);  // Block parent so that child can schedule first
    printf("hello from parent");
    exit(EXIT_SUCCESS);
  }

}
