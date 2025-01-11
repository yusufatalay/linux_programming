#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "uthash.h"

/*Maximum size of messages exchanged between client and server*/
#define BUF_SIZE  512                                                             

/*Server port number*/
#define SERVER_PORT_NUM 50002 


