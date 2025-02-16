#include "dgram.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  struct sockaddr_un svaddr, claddr;

  int sfd, j;
  ssize_t numBytes;
  socklen_t len;
  char buf[BUF_SIZE];

  sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* construct well-known address and bind server socket to it*/
  if (strlen(SV_SOCK_PATH) > sizeof(svaddr.sun_path) - 1) {
    fprintf(stderr, "Server socket path too long: %s", SV_SOCK_PATH);
    exit(EXIT_FAILURE);
  }

  if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
    perror("remove");
    exit(EXIT_FAILURE);
  }

  memset(&svaddr, 0, sizeof(struct sockaddr_un));
  svaddr.sun_family = AF_UNIX;
  strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /* Receive messages, convrt to uppercase, and return to client*/

  for (;;) {
    // sleep so that client can send as many messages as possible to cause bottleneck 
    sleep(10);
    len = sizeof(struct sockaddr_un);
    numBytes =
        recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *)&claddr, &len);
    if (numBytes == -1) {
      perror("recvfrom");
      exit(EXIT_FAILURE);
    }

    printf("Server received %ld bytes from %s\n", (long)numBytes,
           claddr.sun_path);

    for (j = 0; j < numBytes; j++) {
      buf[j] = toupper((unsigned char)buf[j]);
    }

    if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *)&claddr, len) !=
        numBytes) {
      perror("sendto");
      exit(EXIT_FAILURE);
    }
  }
}
