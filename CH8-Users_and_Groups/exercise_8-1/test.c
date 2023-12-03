#include <pwd.h>
#include <stdio.h>
int main(int argc, char *argv[]) {

  char *me = getpwuid(1000)->pw_name;
  char *root = getpwuid(0)->pw_name;
  printf("%s %s\n", me, root);
}
