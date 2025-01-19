#define _DEFAULT_SOURCE
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* returns a pointer to a pathname on
   success.  On error, NULL is returned, and errno is set to
   indicate the error.
*/
#include <stdio.h>
char *ttyname(int fd) {
  static char buf[PATH_MAX];
  char procPath[64];
  ssize_t n;

  struct termios t;

  /* First, check if fd is associated with a terminal via termios. */
  if (tcgetattr(fd, &t) < 0) {
    /* Not a terminal or invalid fd. */
    errno = ENOTTY;
    return NULL;
  }

  /*
   * On Linux, /proc/self/fd/<fd> is a symlink to the file (device) opened
   * by that file descriptor. We'll read that link to get the path.
   */
  snprintf(procPath, sizeof(procPath), "/proc/self/fd/%d", fd);

  n = readlink(procPath, buf, sizeof(buf) - 1);
  if (n < 0) {
    /* readlink failed, possibly no /proc or permission issues. */
    return NULL;
  }
  buf[n] = '\0'; /* Null-terminate the string. */

  return buf;
}
