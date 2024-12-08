#include "error_functions.h"
#include "fifo_seqnum.h"
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
  int serverFd, dummyFd, clientFd, backupFd;

  char clientFIFO[CLIENT_FIFO_NAME_LEN];
  char backupFileContent[21];   // max number of digits for a 64 bit number is 20
  struct request req;
  struct response resp;
  int seqNum = 0; // This is our service
  // check if backup file exists
  // if so read seqNum from that file
  // else create it
  // update the file content with each seqNum update

  if((backupFd = open(SEQNUM_BAK_FILE, O_RDWR | O_SYNC)) == -1 ){
    if (errno == ENOENT){
      if (creat(SEQNUM_BAK_FILE, 0666) == -1){
        errExit("creat %s", SEQNUM_BAK_FILE);
      }
    }else{
      errExit("open %s", SEQNUM_BAK_FILE);
    }
  }

  switch(read(backupFd, backupFileContent, 21)){
    case -1:
      errExit("read %s", SEQNUM_BAK_FILE);
    case 0:
      // file just craeted nothing inside of it
      break;
    default:
      // file has some value in it, try to conver it to number and assign it to seqNum
      errno = 0;    /* To distinguish success/failure after call */
      seqNum = strtol(backupFileContent, NULL, 10);
      if (errno == EINVAL) {
       errExit("strtol %s", SEQNUM_BAK_FILE);
      }
      break;
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
    // update backup file's content as-well
    // move cursor to the beginning of the file
    lseek(backupFd,0 , SEEK_SET);
    snprintf(backupFileContent, 21, "%d", seqNum);
    if (write(backupFd, backupFileContent, 21 ) == -1){
      fprintf(stderr, "Error writing to backup file %s \n", backupFileContent);
    }
  }

  if (close(backupFd) == -1) {
    errMsg("close");
  }
}
