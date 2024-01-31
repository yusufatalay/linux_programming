#define _GNU_SOURCE 600
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
void my_abort() {
  struct sigaction sa;

  // acquire current disposition for the sigabrt
  if (sigaction(SIGABRT, NULL, &sa) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  // create a new signal set
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGABRT);

  // unblock the SIGABRT
  if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  // raise abort signal
  if (raise(SIGABRT) != 0) {
    perror("raise");
    exit(EXIT_FAILURE);
  }

  // if we're here, then the signal was ignored
  // flush all open output streams, remove any created temporary files
  fflush(NULL);

  // restore the default disposition of the abort signal
  signal(SIGABRT, SIG_DFL);

  // raise abort signal again
  if (raise(SIGABRT) != 0) {
    perror("raise");
    exit(EXIT_FAILURE);
  }
}

int main(void) {
  printf("TESTING MY ABORT\n");
  my_abort();
  printf("MY ABORT DID NOT WORK CORRECTLY\n");
}
