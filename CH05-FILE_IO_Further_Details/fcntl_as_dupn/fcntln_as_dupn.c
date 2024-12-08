#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int mydup(int oldfd) {
  int newfd;

  newfd = fcntl(oldfd, F_DUPFD);
  if (newfd == -1) {
    perror("mydup");
    exit(EXIT_FAILURE);
  }

  close(oldfd);
  return newfd;
}


int mydup2(int oldfd, int newfd) {
  // check if oldfd == newfd
  if (oldfd == newfd) {
    return newfd;
  }

  // check if oldfd is valid
  if (fcntl(oldfd, F_GETFL) == -1) {
    perror("mydup2");
    exit(EXIT_FAILURE);
  }

  // close newfd if it's already open
  if (fcntl(newfd, F_GETFL) != -1) {
    close(newfd);
  }

  int res = fcntl(oldfd, F_DUPFD, newfd);
  if (res == -1) {
    perror("mydup2");
    exit(EXIT_FAILURE);
  }

  close(oldfd);
  return newfd;
}
