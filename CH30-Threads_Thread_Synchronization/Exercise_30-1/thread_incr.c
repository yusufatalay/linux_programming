#include "error_functions.h"
#include "get_num.h"
#include "tlpi_hdr.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static volatile int glob = 0; // volatile prevents compiler optimization
                              // of arithmetic operations on glob

typedef struct threadParam {
  int loops;
  char *tid;
} threadParam;

static void * // loop 'arg' times incrementing 'glob'
threadFunc(void *arg) {
  threadParam param = *((threadParam *)arg);
  int loc, j;

  printf("%s: Current value of glob is %d\n", param.tid, glob);

  for (j = 0; j < param.loops; j++) {
    loc = glob;
    loc++;
    glob = loc;

    printf("%s: Current value of glob is %d\n", param.tid, glob);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t t1, t2;
  int loops, s;

  loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;

  threadParam *param1 = (threadParam *)malloc(sizeof(threadParam));
  if (param1 == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  threadParam *param2 = (threadParam *)malloc(sizeof(threadParam));
  if (param1 == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  param1->loops = loops;
  param1->tid = strdup("Thread 1");

  param2->loops = loops;
  param2->tid = strdup("Thread 2");

  s = pthread_create(&t1, NULL, threadFunc, param1);
  if (s != 0) {
    errExitEN(s, "pthread_create");
  }

  s = pthread_create(&t2, NULL, threadFunc, param2);
  if (s != 0) {
    errExitEN(s, "pthread_create");
  }

  s = pthread_join(t1, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_join");
  }

  s = pthread_join(t2, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_join");
  }

  printf("glob = %d\n", glob);
  exit(EXIT_SUCCESS);
}
