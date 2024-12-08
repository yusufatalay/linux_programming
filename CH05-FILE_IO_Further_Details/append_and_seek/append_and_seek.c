#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    printf("Usage: %s file\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int fD;
  // open the file
  fD = open(argv[1], O_APPEND | O_RDWR);
  if (fD == -1) {
    printf("cannot open file\n");
    exit(EXIT_FAILURE);
  }

  char buf[] = "THIS IS THE TEXT TO APPEND";

  // seek to the top of the file
  off_t offset = lseek(fD, 0, SEEK_SET);
  if (offset == -1) {
    printf("lseeek error\n");
    exit(EXIT_FAILURE);
  }

  int writeAmount = write(fD, buf, 27);
  if (writeAmount == -1 || writeAmount != 27) {
    printf("write error or partial write\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
