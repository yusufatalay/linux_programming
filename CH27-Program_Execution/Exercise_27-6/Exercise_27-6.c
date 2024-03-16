#include <bits/types/sigset_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

void sighandler(int signum) {
  printf("handler\n");
  return;
}
int main(void) {
  struct sigaction sa;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = sighandler;
  // Establish signal handler for SIGCHLD
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  sigset_t blockset;
  sigemptyset(&blockset);
  sigaddset(&blockset, SIGCHLD);
  // Block SIGCHLD for this process and its child
  sigprocmask(SIG_BLOCK, &blockset, NULL);

  int status;
  switch (fork()) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0: // Child
    printf("I am child, exiting now\n");
    _exit(EXIT_SUCCESS);
  default:    // Parent
    sleep(5); // sleep so that child can schedule first
    printf("I am parent, my child is gone :(\n");
    // unblock SIGCHLD
    wait(&status);
    printf("SIGCHLD unblocked by parent\n");
    sigprocmask(SIG_UNBLOCK, &blockset, NULL);
    printf("Child's exit status: %ld\n", (long)status);
    exit(EXIT_SUCCESS);
  }
}
