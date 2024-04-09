#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_once_t basenameOnce = PTHREAD_ONCE_INIT;
static pthread_key_t basenameKey;

static pthread_once_t dirnameOnce = PTHREAD_ONCE_INIT;
static pthread_key_t dirnameKey;

static void destructor(void *buf) { free(buf); }

static void createKeyBaseName(void) {
  int s;

  s = pthread_key_create(&basenameKey, destructor);
  if (s != 0) {
    perror("pthread_key_create");
    exit(EXIT_FAILURE);
  }
}

static void createKeyDirName(void) {
  int s;

  s = pthread_key_create(&dirnameKey, destructor);
  if (s != 0) {
    perror("pthread_key_create");
    exit(EXIT_FAILURE);
  }
}

char *basename_ts(char *path) {
  int s;
  char *buf;

  s = pthread_once(&basenameOnce, createKeyBaseName);
  if (s != 0) {
    perror("pthread_once");
    exit(EXIT_FAILURE);
  }

  buf = pthread_getspecific(basenameKey);
  if (buf == NULL) {
    buf = malloc(strlen(path));
    if (buf == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }
  }

  s = pthread_setspecific(basenameKey, buf);
  if (s != 0) {
    perror("pthread_setspecific");
    exit(EXIT_FAILURE);
  }

  if (path == NULL || strcmp(path, "") == 0) {
    strcpy(buf, ".");
    return buf;
  }

  if (path[strlen(path) - 1] == '/') {
    path[strlen(path) - 1] = '\0';
  }

  char *slash;
  slash = strrchr(path, '/');
  if (slash == NULL) {
    strcpy(buf, path);
  } else {

    strcpy(buf, slash + 1);
  }
  return buf;
}

char *dirname_ts(char *path) {

  int s;
  char *buf;

  s = pthread_once(&basenameOnce, createKeyBaseName);
  if (s != 0) {
    perror("pthread_once");
    exit(EXIT_FAILURE);
  }

  buf = pthread_getspecific(basenameKey);
  if (buf == NULL) {
    buf = malloc(strlen(path));
    if (buf == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }
  }

  s = pthread_setspecific(basenameKey, buf);
  if (s != 0) {
    perror("pthread_setspecific");
    exit(EXIT_FAILURE);
  }

  if (path == NULL || strcmp(path, "") == 0) {
    strcpy(buf, ".");
    return buf;
  }

  if (path[strlen(path) - 1] == '/') {
    path[strlen(path) - 1] = '\0';
  }

  char *slash;
  slash = strrchr(path, '/');
  if (slash != NULL) {
    *slash = '\0'; // terminate the result at last slash
  }
  strcpy(buf, path);
  return buf;
}
