#ifndef MQ_SEQNUM_H
#define MQ_SEQNUM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_MQ "/mq-seqnum_sv"                                 /* Well-known name for server's MQ        */
#define CLIENT_MQ_TEMPLATE "/mq-seqnum_cl.%ld"                    /* Template for building client MQ name   */
#define CLIENT_MQ_NAME_LEN (sizeof(CLIENT_MQ_TEMPLATE) + 20)     /* Space required for client MQ pathname  */

struct request { /*Request (client -> server)*/  
  pid_t pid; /*PID of client*/ 
  int seqLen; /*Length of desired sequence*/
};

struct response{  /* Response (server -> client) */
  int seqNum;      /* start of sequence */
};

#endif
