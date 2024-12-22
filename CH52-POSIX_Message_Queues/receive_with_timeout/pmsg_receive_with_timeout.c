#include <bits/time.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static void usageError(const char *progName) {
  fprintf(stderr, "Usage: %s [-n] name [-t] seconds\n", progName);
  fprintf(stderr, "\t-n\t\tUse O_NONBLOCK flag\n");
  fprintf(stderr, "\t-t\t\tTimeout in seconds\n");
  exit(EXIT_FAILURE);
}

void getAbsTime(int relSecond, struct timespec *abstime) {
  if (clock_gettime(CLOCK_REALTIME, abstime) == -1) {
    perror("clock_gettime");
    exit(EXIT_FAILURE);
  }

  abstime->tv_sec += relSecond;
}

int main(int argc, char *argv[]) {
  int flags, opt;
  mqd_t mqd;
  unsigned int prio;
  void *buffer;
  struct mq_attr attr;
  ssize_t numRead;
  unsigned int timeout;
  struct timespec tS;
  flags = O_RDONLY;

  while ((opt = getopt(argc, argv, "nt:")) != -1) {
    switch (opt) {
    case 'n':
      flags |= O_NONBLOCK;
      break;
    case 't':
      timeout = atoi(optarg);
      break;
    default:
      usageError(argv[0]);
    }
  }

  if (optind >= argc) {
    usageError(argv[0]);
  }

  mqd = mq_open(argv[optind], flags);
  if (mqd == (mqd_t)-1) {
    perror("mqd_open");
    exit(EXIT_FAILURE);
  }

  if (mq_getattr(mqd, &attr) == -1) {
    perror("mq_getattr");
    exit(EXIT_FAILURE);
  }

  buffer = malloc(attr.mq_msgsize + 1);
  if (buffer == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  getAbsTime(timeout, &tS);
  numRead = mq_timedreceive(mqd, buffer, attr.mq_msgsize, &prio, &tS);
  if (numRead == -1) {
    perror("mq_receive");
    exit(EXIT_FAILURE);
  }

  printf("Read %ld bytes; priority = %u\n", (long)numRead, prio);
  if (write(STDOUT_FILENO, buffer, numRead) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  write(STDOUT_FILENO, "\n", 1);
  exit(EXIT_SUCCESS);
}
