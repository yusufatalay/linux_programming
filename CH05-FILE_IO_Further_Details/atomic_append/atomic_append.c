#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  // parse command line params
  // first parameter is filename (string)
  // second one is amount of bytes (long)
  // last one is flag (x)          // (bool)

  if ((argc < 3 || argc > 5) || strcmp(argv[1], "--help") == 0) {
    printf("Usage: %s filename numBytes [x]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int fd;
  long numBytes = 0;
  int flag = O_CREAT | O_RDWR;

  if (argc == 4 && strcmp(argv[3], "x") == 0) {
    flag |= O_APPEND;
    fd = open(argv[1], flag, 0666);
  } else {
    fd = open(argv[1], flag, 0666);
  }

  if (fd == -1) {
    perror("could not open file");
    exit(EXIT_FAILURE);
  }

  numBytes = strtol(argv[2], NULL, 10);
  for (int i = 0; i < numBytes; i++) {
    char character = (i + 33) % 127;
    if (!(flag & O_APPEND)) {
      lseek(fd, 0, SEEK_END);
    }
    if (write(fd, &character, 1) != 1) {
      printf("could not write the file\n");
      exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
}
