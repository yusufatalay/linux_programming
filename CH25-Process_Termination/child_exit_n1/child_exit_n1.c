#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  int childExitStatus = 0;

  switch (fork()) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    printf("Child with pid : %d about to exit with -1 status\n", getpid());
    exit(-1);
  default:
    wait(&childExitStatus);
    printf("Child exited with %d\n", childExitStatus);
    printf("Child exited with %d\n", WEXITSTATUS(childExitStatus));

    exit(EXIT_SUCCESS);
  }
}
