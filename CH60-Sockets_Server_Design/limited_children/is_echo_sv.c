#include "become_daemon.h"
#include "inet_sockets.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <syslog.h>
#include <unistd.h>

/*Name of TCP service*/
#define SERVICE "echo"
#define BUF_SIZE 4096

#define MAX_CHILD 10

static int CHILD_COUNT = 0;

/*SIGCHLD handler to read dead child processes*/
static void grimReaper(int sig) {
  int savedErrno; /*Save errno in case changed here*/

  savedErrno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0) {
    CHILD_COUNT--;
    continue;
  }

  errno = savedErrno;
}

/*Handle a client request: copy socket input back to socket*/
static void handleRequest(int cfd) {
  char buf[BUF_SIZE];
  ssize_t numRead;

  while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
    if (write(cfd, buf, numRead) != numRead) {
      syslog(LOG_ERR, "write() failed: %s", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  if (numRead == -1) {
    syslog(LOG_ERR, "Error from read():  %s", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  int lfd, cfd; /*Listening and connected sockets*/
  struct sigaction sa;

  if (becomeDaemon(0) == -1) {
    perror("becomeDaemon");
    exit(EXIT_FAILURE);
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = grimReaper;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  lfd = inetListen(SERVICE, 10, NULL);
  if (lfd == -1) {
    syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
    exit(EXIT_FAILURE);
  }

  for (; CHILD_COUNT != MAX_CHILD;) {

    cfd = accept(lfd, NULL, NULL); /*Wait for connection */
    if (cfd == -1) {
      syslog(LOG_ERR, "Failure in accept(): %s", strerror(errno));
      exit(EXIT_FAILURE);
    }

    /*Hanel each client request in a new child process*/
    switch (fork()) {
    case -1:
      syslog(LOG_ERR, "Can't create child (%s)", strerror(errno));
      close(cfd); /*Give up on this client*/
      break;      /* May be temporary; try nex client */

    case 0:
      close(lfd); /*Unneeded copy of listening socket*/
      handleRequest(cfd);
      _exit(EXIT_SUCCESS);
    default:
      CHILD_COUNT++;
      close(cfd); /*Unneeded copy of connected socket*/
      break;
    }
  }
}
