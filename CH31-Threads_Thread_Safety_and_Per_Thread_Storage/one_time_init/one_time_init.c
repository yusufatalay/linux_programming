#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads.h>

typedef struct control {
  bool fired;
  pthread_mutex_t controlMtx;

} control;

int one_time_init(control *ctrl, void (*init)(void)) {

  pthread_mutex_lock(&ctrl->controlMtx);
  if (!ctrl->fired) {
    ctrl->fired = true;
    init();
  }
  pthread_mutex_unlock(&ctrl->controlMtx);

  return 0;
}

void initfunc(void) { printf("HELLO \n"); }
