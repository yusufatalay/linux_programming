#include "mq_seqnum.h"
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "get_num.h"

static char clientMQ[CLIENT_MQ_NAME_LEN];

/*Invoked on exit to delete client MQ*/
static void removeMQ(void) { mq_unlink(clientMQ); }

int main(int argc, char *argv[]) {
  int serverMQd, clientMQd;
  struct request req;
  struct response resp;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    fprintf(stderr,"%s [seq-len]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Create our MQ (before sending request, to avoid a race)*/

  umask(0);
  snprintf(clientMQ, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
           (long)getpid());

  clientMQd = (mq_open(clientMQ, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP));
  if (clientMQd == -1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  if (atexit(removeMQ) != 0) {
    perror("atexit");
    exit(EXIT_FAILURE);
  }

  /* Construct request message, open server FIFO, and send request */
  req.pid = getpid();
  req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

  serverMQd = mq_open(SERVER_MQ, O_WRONLY);
  if (serverMQd == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  if (mq_send(serverMQd, &req, sizeof(struct request), 0) != sizeof(struct request)) {
    perror("mq_send");
    exit(EXIT_FAILURE);
  }

  if (mq_receive(clientMQd, &resp, sizeof(struct response), 0) !=
      sizeof(struct response)) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
  }

  printf("%d\n", resp.seqNum);
  exit(EXIT_SUCCESS);
}
