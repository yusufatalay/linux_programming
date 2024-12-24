#include <bits/time.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "get_num.h"

int main(int argc, char *argv[]){
  sem_t *sem;
  int timeoutSec;
  struct timespec timeout;

  if(argc < 3 || strcmp(argv[0], "--help") == 0){
    fprintf(stderr,"%s sem-name timeout-sec\n", argv[0] );
    exit(EXIT_FAILURE);
  }

  timeoutSec = getInt(argv[2],GN_GT_0 ,"timeout-sec" );

  if(clock_gettime(CLOCK_REALTIME, &timeout) == -1){
    perror("clock_gettime");
    exit(EXIT_FAILURE);
  }

  timeout.tv_sec += timeoutSec;

  sem = sem_open(argv[1], 0);
  if(sem == SEM_FAILED){
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  if(sem_timedwait(sem,&timeout) == -1){
    perror("sem_wait");
    exit(EXIT_FAILURE);
  }

  printf("%ld sem_wait() succeeded\n", (long)getpid());
  exit(EXIT_SUCCESS);
}
