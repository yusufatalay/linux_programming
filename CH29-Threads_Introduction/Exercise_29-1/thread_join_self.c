#include <pthread.h>
#include <stdio.h>

void *threadFunc(void *arg) {
  pthread_join(pthread_self(), NULL);
  return 0;
}

int main(void) {
  pthread_t tid;
  pthread_create(&tid, NULL, threadFunc, NULL);

}
