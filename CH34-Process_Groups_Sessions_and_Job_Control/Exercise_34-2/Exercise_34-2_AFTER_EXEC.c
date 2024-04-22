#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  pid_t childPid;
  char *sleepArgs[] = {"sleep", "10", NULL};

  childPid = fork();
  switch (childPid) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    if (execvp("/usr/bin/sleep", sleepArgs) == -1) {
      perror("execvp");
      exit(EXIT_FAILURE);
    }

  default:
    sleep(10); // Make sure child runs exec first
    printf("Hello from parent my PGID: %ld\n", (long)getpgrp());
    if (setpgid(childPid, childPid) != 0) {
      perror("setpgid");
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
  }
}
