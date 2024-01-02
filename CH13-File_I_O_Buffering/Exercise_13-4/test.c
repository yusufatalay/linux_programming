#include <fcntl.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  printf("If I had mode time, ");
  write(STDOUT_FILENO, "I would have written you a shorter letter.\n", 43);
  int fd;

  fd = open("testoutput.txt", O_RDWR | O_CREAT,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  if (dup2(fd, STDOUT_FILENO) == -1) {
    perror("dup2");
    exit(EXIT_FAILURE);
  }

  printf("If I had mode time, ");
  write(STDOUT_FILENO, "I would have written you a shorter letter.\n", 43);
}
