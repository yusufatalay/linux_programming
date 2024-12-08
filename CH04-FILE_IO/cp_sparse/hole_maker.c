#include "error_functions.h"
#include "tlpi_hdr.h"
#include <fcntl.h>
int main() {
  int fd;

  if ((fd = open("./test_intput_with_hole", O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0) {
    errExit("creat error");
  }

  if (write(fd, "ABCDE", 5) != 5)
    errExit("buf1 write error");

  if (lseek(fd, 40, SEEK_SET) == -1)
    errExit("lseek error");

  if (write(fd, "abcde", 5) != 5)
    errExit("buf2 write error");

  return 0;
}
