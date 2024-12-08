#ifndef FIFO_SEQNUM_H
#define FIFO_SEQNUM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define SERVER_FIFO "/tmp/seqnum_sv"                                 /* Well-known name for server's FIFO        */
#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"                    /* Template for building client FIFO name   */
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)     /* Space required for client FIFO pathname  */
#define SEQNUM_BAK_FILE "/tmp/seqnum_bak"                            /* Well-known name for storing seqnum for further reads*/

struct request { /*Request (client -> server)*/  
  pid_t pid; /*PID of client*/ 
  int seqLen; /*Length of desired sequence*/
};

struct response{  /* Response (server -> client) */
  int seqNum;      /* start of sequence */
};

#endif
