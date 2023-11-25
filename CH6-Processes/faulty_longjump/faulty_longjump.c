#include <setjmp.h>
#include <stdio.h>

static jmp_buf env;

void x(void) {
  if (setjmp(env) == 0) {
    printf("In x() setjmp called first time\n");
  } else {
    printf("In x() longjmp called, control returned here\n");
  }
}

int main(void) {
  x();
  longjmp(env, 1);
  printf("Back in main()\n");
  return 0;
}
