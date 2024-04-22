#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  if (setsid() != 0) {
    perror("setsid");
    exit(EXIT_FAILURE);
  }
}
