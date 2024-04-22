#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  pid_t childPid;

  childPid = fork();
  switch (childPid) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    sleep(10); // Make sure parent changes the PGID first
    printf("Hello from child my PGID: %ld\n", (long)getpgrp());
  default:
    if (setpgid(childPid, childPid) != 0) {
      perror("setpgid");
      exit(EXIT_FAILURE);
    }
    printf("Hello from parent my PGID: %ld\n", (long)getpgrp());
    waitpid(childPid, NULL, 0);

    exit(EXIT_SUCCESS);
  }
}
