#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include "mq_seqnum.h"

int main(void) {
  mqd_t serverMQd, clientMQd;
  struct mq_attr serverMQAttr;
  char clientMQ[CLIENT_MQ_NAME_LEN];
  struct request req;
  struct response resp;
  int seqNum = 0; // This is our service
  size_t numRead;

  // set the server's mq
  serverMQAttr.mq_maxmsg= 10;
  serverMQAttr.mq_msgsize = sizeof(struct request);

  umask(0); /* So we get the permission we want*/

  serverMQd = mq_open(SERVER_MQ, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &serverMQAttr );
  if (serverMQd == (mqd_t) -1){
    perror("mq_open");
    exit(EXIT_FAILURE);
  }


  for (;;) { // Read requests and send responses
    
    numRead = mq_receive(serverMQd, &req, serverMQAttr.mq_msgsize, 0  );
    if (numRead == -1 ){
      perror("mq_receive");
      exit(EXIT_FAILURE);
    }


    /* Open client FIFO (previously created by client) */
    snprintf(clientMQ, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
             (long)req.pid);
    clientMQd = mq_open(clientMQ, O_WRONLY);
    if (clientMQd == (mqd_t)-1) {
      perror("mq_open");
      continue;
    }
    /* Send response and close FIFO */

    resp.seqNum = seqNum;

    if (mq_send(clientMQd, &resp, sizeof(struct response), 0) !=
        sizeof(struct response)) {
      fprintf(stderr, "Error writing to MQ %s \n", clientMQ);
    }
    if (mq_close(clientMQd) == -1) {

    }

    seqNum += req.seqLen; // Update our sequence number
  }

  mq_unlink(SERVER_MQ);

}
