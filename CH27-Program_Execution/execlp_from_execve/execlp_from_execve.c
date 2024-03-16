#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int my_execlp(const char *file, const char *arg, ...) {
  int argvCap = 10; // argument vector capacity
  int argc = 0;     // argument count
  char **argv;      // arg list
  char **envp;      // caller's environment
  int envSize;

  for (envSize = 1; environ[envSize] != NULL;) {
    envSize++;
  }
  envp = calloc(envSize, sizeof(void *));
  if (envp == NULL) {
    perror("calloc");
    return -1;
  }

  // duplicate the environment
  int i;
  for (i = 0; environ[i] != NULL; i++) {
    envp[i] = strdup(environ[i]);
  }
  envp[i] = NULL; // Null terminate the environment

  // capture the argument list
  argv = calloc(argvCap, sizeof(void *));
  if (argv == NULL) {
    perror("calloc");
    free(envp);
    return -1;
  }

  argv[0] = (char *)arg;
  argc++;

  va_list argList;
  va_start(argList, arg);
  while (argv[argc - 1] != NULL) {
    if (argc + 1 > argvCap) {
      argvCap += 5;
      char **newargv = realloc(argv, argvCap * sizeof(void *));
      if (newargv == NULL) {
        perror("realloc");
        free(envp);
        return -1;
      } else {
        argv = newargv;
      }
    }
    argv[argc] = va_arg(argList, char *);
    argc++;
  }
  va_end(argList);

  // at this point we have argv and envp to call the execve
  // now we need to tokenize the PATH env

  // conventionally if the file parameter contains a forward slash (/) do not
  // use PATH
  if (strchr(file, '/') != NULL) {
    execve(file, argv, envp);
  } else {
    char *path = getenv("PATH");
    char *PATH;
    if (path == NULL || strlen(path) == 0) {
      PATH = strdup("/usr/bin");
    } else {
      PATH = strdup(path);
    }

    char *pathname;

    char *token = strtok(PATH, ":");
    while (token != NULL) {
      pathname = calloc(1, strlen(token) + strlen(file) + 1);
      if (pathname == NULL) {
        perror("malloc");
        free(envp);
        return -1;
      }
      strncat(pathname, token, strlen(token));
      // check if token ends with a / if not add one
      if (token[strlen(token) - 1] != '/') {
        strncat(pathname, "/", 1);
      }
      strncat(pathname, file, strlen(file));

      execve(pathname, argv, envp);
      token = strtok(NULL, ":");
    }
    free(pathname);
  }

  return 0;
}

int main(void) { my_execlp("vim", "-p2"); }
