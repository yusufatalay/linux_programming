#include "error_functions.h"
#include "tlpi_hdr.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[]) {

  // argument validation
  if (argc != 3 && strcmp(argv[1], "--help") == 0) {
    printf("Usage:\t%s srcfile dstfile", argv[0]);
    exit(EXIT_FAILURE);
  }
  int sF; // source file dsc
  int dF; // dst file dsc

  sF = open(argv[1], O_RDONLY);
  if (sF == -1) {
    errExit("open source file");
  }

  dF = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (dF == -1) {
    errExit("open destination file");
  }

  // read from source
  char buf[BUF_SIZE];
  ssize_t readAmount;
  size_t nullSeqLen;

  while ((readAmount = read(sF, buf, BUF_SIZE)) > 0) {
    // check for null byte sequences
    for (int i = 0; i < readAmount; i++) {
      if (buf[i] == '\0') {
        nullSeqLen++;
      } else if (nullSeqLen > 0) {
        lseek(dF, nullSeqLen, SEEK_CUR);
        write(dF, &buf[i], 1);
        nullSeqLen = 0;
      } else {
        write(dF, &buf[i], 1);
      }
    }
  }


  if (close(sF) == -1) {
    errExit("close input");
  }

  if (close(dF) == -1) {
    errExit("close output");
  }

  exit(EXIT_SUCCESS);
}
