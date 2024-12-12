#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "Usage: %s source destination\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *sourceMap, *destinationMap;
  int sourceFD, destinationFD;
  // open source file
  sourceFD = open(argv[1], O_RDONLY);
  if (sourceFD == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  // get source file's size
  struct stat sourceFileStat;
  if (fstat(sourceFD, &sourceFileStat) == -1) {
    perror("fstat");
    exit(EXIT_FAILURE);
  }

  // map the source file
  sourceMap =
      mmap(NULL, sourceFileStat.st_size, PROT_READ, MAP_PRIVATE, sourceFD, 0);
  if (sourceMap == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  // close the source file, it is not necessary anymore
  if (close(sourceFD) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  // open destination file, create if not exist
  destinationFD = open(argv[2], O_RDWR | O_CREAT);
  if (destinationFD == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  // truncate destination file's length
  if (ftruncate(destinationFD, sourceFileStat.st_size) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }

  // map destination file
  destinationMap = mmap(NULL, sourceFileStat.st_size, PROT_WRITE | PROT_READ,
                        MAP_SHARED, destinationFD, 0);
  if (destinationMap == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  memcpy(destinationMap, sourceMap, sourceFileStat.st_size);

  msync(destinationMap, sourceFileStat.st_size, MS_SYNC);

  if (munmap(sourceMap, sourceFileStat.st_size) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }

  if (munmap(destinationMap, sourceFileStat.st_size) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
