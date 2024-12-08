#include "fifo_seqnum.h"

static char clientFIFO[CLIENT_FIFO_NAME_LEN];

/*Invoked on exit to delete client FIFO*/
static void removeFIFO(void) { unlink(clientFIFO); }

int main(int argc, char *argv[]) {
  int serverFd, clientFd;
  struct request req;
  struct response resp;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [seq-len]\n", argv[0]);
  }

  /* Create our FIFO (before sending request, to avoid a race)*/

  umask(0);
  snprintf(clientFIFO, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
           (long)getpid());

  if ((mkfifo(clientFIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1) &&
      errno != EEXIST) {
    errExit("mkfifo %s", clientFIFO);
  }

  if (atexit(removeFIFO) != 0) {
    errExit("atexit");
  }

  /* Construct request message, open server FIFO, and send request */
  req.pid = getpid();
  req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

  serverFd = open(SERVER_FIFO, O_WRONLY);
  if (serverFd == -1) {
    errExit("open %s", SERVER_FIFO);
  }

  if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
    fatal("Can't write to server");
  }

  /* Open our FIFO, read and display response */
  clientFd = open(clientFIFO, O_RDONLY);
  if (clientFd == -1) {
    errExit("open %s", clientFIFO);
  }

  if (read(clientFd, &resp, sizeof(struct response)) !=
      sizeof(struct response)) {
    fatal("Can't read response from server");
  }

  printf("%d\n", resp.seqNum);
  exit(EXIT_SUCCESS);
}
