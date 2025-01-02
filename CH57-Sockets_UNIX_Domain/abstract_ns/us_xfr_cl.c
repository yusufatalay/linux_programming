#include "us_xfr.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  int sfd;
  ssize_t numRead;
  char buf[BUF_SIZE];

  sfd = socket(AF_UNIX, SOCK_STREAM, 0); /* Create client socket*/
  if (sfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Construct server address, and make the connection*/

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) ==
      -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  /* Copy stdin to socket*/
  while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
    if (write(sfd, buf, numRead) != numRead) {
      perror("partial/failed write");
      exit(EXIT_FAILURE);
    }
  }

  if (numRead == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);  /* Closes our socket; server sees EOF*/
}
