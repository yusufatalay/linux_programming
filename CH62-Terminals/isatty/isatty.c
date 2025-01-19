#include <errno.h>
#include <termios.h>

/*returns 1 if fd is an open file descriptor referring to
 a terminal; otherwise 0 is returned, and errno is set to indicate
 the error.
*/
int isatty(int fd) {
  struct termios t;

  if (tcgetattr(fd, &t) == -1) {
    if (errno == ENOTTY) {
      errno = 0;  // this is not a terminal does not indicates an error for isatty
    }

    return 0;
  }
  return 1;
}
