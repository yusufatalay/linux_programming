#define _XOPEN_SOURCE 700
#include "error_functions.h"
#include "fifo_seqnum.h"
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

static void fifo_sig_handler(int signum){
  write(STDOUT_FILENO, "SIGNALLED\n", 10);
  exit(-1);
}
int main(void) {
  int serverFd, dummyFd, clientFd;
  char clientFIFO[CLIENT_FIFO_NAME_LEN];
  struct request req;
  struct response resp;
  int seqNum = 0; // This is our service

  // handle SIGINT and SIGTERM
  struct sigaction sa;
  sa.sa_handler = fifo_sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART; /* Restart functions if
                               interrupted by handler */
  if (sigaction(SIGINT, &sa, NULL) == -1){
    errExit("sigaction register SIGINT");
  }

  if (sigaction(SIGTERM, &sa, NULL) == -1){
    errExit("sigaction register SIGTERM");
  }

  /* Creat well-known FIFO, and open it for reading */
  umask(0); /* So we get the permission we want*/

  if ((mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1) &&
      errno != EEXIST) {
    errExit("mkfifo %s", SERVER_FIFO);
  }
  serverFd = open(SERVER_FIFO, O_RDONLY);
  if (serverFd == -1) {
    errExit("open %s", SERVER_FIFO);
  }

  /* Open an extra write descriptor, so that we never see EOF */

  dummyFd = open(SERVER_FIFO, O_WRONLY);
  if (dummyFd == -1) {
    errExit("open %s", SERVER_FIFO);
  }

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    errExit("signal");
  }

  for (;;) { // Read requests and send responses
    if (read(serverFd, &req, sizeof(struct request)) !=
        sizeof(struct request)) {
      fprintf(stderr, "Error reading request; discarding\n");
      continue;
    }

    /* Open client FIFO (previously created by client) */
    snprintf(clientFIFO, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
             (long)req.pid);
    clientFd = open(clientFIFO, O_WRONLY);
    if (clientFd == -1) {
      errMsg("open %s", clientFIFO);
      continue;
    }
    /* Send response and close FIFO */

    resp.seqNum = seqNum;
    if (write(clientFd, &resp, sizeof(struct response)) !=
        sizeof(struct response)) {
      fprintf(stderr, "Error writing to FIFO %s \n", clientFIFO);
    }
    if (close(clientFd) == -1) {
      errMsg("close");
    }

    seqNum += req.seqLen; // Update our sequence number
  }
}
