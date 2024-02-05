#define _GNU_SOURCE // Get strsignal() from string header
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handler(int sigNum)
{
  // UNSAFE
  if (sigNum > 32)
  {
    printf("Caught realtime signal %d (%s)\n", sigNum, strsignal(sigNum));
  }
  else
  {
    printf("Caugh standard signal %d (%s)\n", sigNum, strsignal(sigNum));
  }
}

int main(void)
{
  // set up signal handler for all signals and block all of them
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  sa.sa_handler = &handler;

  if (sigaction(SIGRTMIN + 1, &sa, NULL) == -1)
  {
    perror("sigaction SIGRTMIN + 1");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGINT, &sa, NULL) == -1)
  {
    perror("sigaction SIGINT");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGRTMIN + 3, &sa, NULL) == -1)
  {
    perror("sigaction SIGRTMIN + 3");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGCONT, &sa, NULL) == -1)
  {
    perror("sigaction SIGCONT");
    exit(EXIT_FAILURE);
  }

  sigset_t signalSet;

  if (sigaddset(&signalSet, SIGRTMIN +1) == -1)
  {
    perror("sigaddset");
    exit(EXIT_FAILURE);
  }

  if (sigaddset(&signalSet, SIGRTMIN +3) == -1)
  {
    perror("sigaddset");
    exit(EXIT_FAILURE);
  }


  if (sigaddset(&signalSet, SIGCONT) == -1)
  {
    perror("sigaddset");
    exit(EXIT_FAILURE);
  }

  if (sigaddset(&signalSet, SIGINT) == -1)
  {
    perror("sigaddset");
    exit(EXIT_FAILURE);
  }

  if (sigprocmask(SIG_BLOCK, &signalSet, NULL) == -1)
  {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  printf("A mix of real-time and standard signals are blocked\n");

  sleep(20);
  // raise signals in different order
  raise(SIGRTMIN + 1); // send a realtime signal
  raise(SIGCONT);      // send a standard signal
  raise(SIGRTMIN + 3); // send a realtime signal
  raise(SIGINT);       // send a standard signal

  if (sigprocmask(SIG_UNBLOCK, &signalSet, NULL) == -1)
  {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  printf("Signals are unblocked\n");

  exit(EXIT_SUCCESS);
}
