#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
  // create two files under /tmp
  int file1FD = open("/tmp/file1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (file1FD == -1) {
    perror("open file1");
    exit(EXIT_FAILURE);
  }
  int file2FD = open("/tmp/file2", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (file2FD == -1) {
    perror("open file2");
    exit(EXIT_FAILURE);
  }

  switch (fork()) {
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    // child process
    if (flock(file1FD, LOCK_EX) == -1) {
      perror("flock child lock file1");
      _exit(EXIT_FAILURE);
    }

    sleep(1); // Simulate delay to trigger contention

    if (flock(file2FD, LOCK_EX | LOCK_NB) == -1) {
      if (errno == EWOULDBLOCK) {
        printf("Child detected a potential deadlock\n");
      } else {
        perror("flock child lock file2");
      }
      _exit(EXIT_FAILURE);
    }

    // Unlock
    flock(file1FD, LOCK_UN);
    flock(file2FD, LOCK_UN);
    _exit(EXIT_SUCCESS);
  }

  // parent process
  if (flock(file2FD, LOCK_EX) == -1) {
    perror("flock parent lock file2");
    exit(EXIT_FAILURE);
  }

  sleep(1); // Simulate delay to trigger contention

  if (flock(file1FD, LOCK_EX | LOCK_NB) == -1) {
    if (errno == EWOULDBLOCK) {
      printf("Parent detected a potential deadlock\n");
    } else {
      perror("flock parent lock file1");
    }
    exit(EXIT_FAILURE);
  }

  // Unlock
  flock(file2FD, LOCK_UN);
  flock(file1FD, LOCK_UN);
  exit(EXIT_SUCCESS);
}
