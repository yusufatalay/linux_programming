#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int main(void) {
  struct termios t;

  if (tcgetattr(STDIN_FILENO, &t) == -1) {
    perror("tcgetattr");
    exit(EXIT_FAILURE);
  }

  if (t.c_lflag & ICANON) {
    // terminal in canonical mode
    printf("Terminal in canonical mode\n");
  } else {
    // terminal in non-canonical mode
    printf("Terminal in non-canonical mode\n");
    printf("TIME: %hhu\nMIN: %hhu\n", t.c_cc[VTIME],
           t.c_cc[VMIN]);
  }

  exit(EXIT_SUCCESS);
}
