#include "error_functions.h"
#include "tlpi_hdr.h"
#include <bits/getopt_core.h>
#include <bits/types/cookie_io_functions_t.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[]) {
  // Check if -a option exists in the arg vector
  // if not execve regular tee

  int opt = getopt(argc, argv, "a:");
  if (opt == -1) {
    errExit("getopts");
  }

  if (opt == 'a') {

    // Handle -a option
    int fileD;
    char buf[BUF_SIZE];

    fileD = open(argv[2], O_RDWR | O_APPEND | O_CREAT,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fileD == -1) {
      errExit("open");
    }

    // read from stdin
    int i = read(STDIN_FILENO, buf, BUF_SIZE);
    if (i < 0) {
      fatal("EOF or error occured when reading from STDIN");
    }

    // append to file
    int j = write(fileD, buf, i);
    if (j == -1) {
      errExit("write");
    }

    close(fileD);
    exit(EXIT_SUCCESS);
  }

  // Handle other options or print an error message
  execve("/usr/bin/tee", argv, NULL);
  errExit("tee");
}
