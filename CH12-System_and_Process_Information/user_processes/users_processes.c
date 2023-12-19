#include <dirent.h>
#include <err.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STATUS_FILE_LINE_LEN 100

uid_t userIDFromName(const char *name) {
  struct passwd *pwd;
  uid_t u;
  char *endptr;

  if (name == NULL || *name == '\0') {
    return -1;
  }

  u = strtol(name, &endptr, 10);
  if (*endptr == '\0') {
    return u;
  }

  pwd = getpwnam(name);
  if (pwd == NULL) {
    return -1;
  }

  return pwd->pw_uid;
}

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    printf("Usage: %s <user_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const char *proc = "/proc/";
  const char *status = "/status";
  uid_t userID = userIDFromName(argv[1]);
  char userIDStr[6];
  if (sprintf(userIDStr, "%d", userID) < 0) {
    printf("cannot convert user's ID to str\n");
    exit(EXIT_FAILURE);
  }

  DIR *procDIR;
  FILE *statusFiled;
  struct dirent *procDirContent;

  char *processStatusFilePath;

  char *endptr = "";

  char statusFileLine[STATUS_FILE_LINE_LEN];

  procDIR = opendir(proc);
  if (procDIR == NULL) {
    perror("opendir");
    exit(EXIT_FAILURE);
  }

  while ((procDirContent = readdir(procDIR)) != NULL) {
    if ((strcmp(procDirContent->d_name, ".") == 0) ||
        (strcmp(procDirContent->d_name, "..") == 0)) {
      continue;
    }
    if (procDirContent->d_type ==
        DT_DIR) { // link files such as self is linked to file in this
                  // directory, so no need to search for it
      // make sure the filename is numeric (all processes have numeric names)
      strtol(procDirContent->d_name, &endptr, 10);
      if (*endptr == '\0') {
        // We've found process directory
        // construct path to that directory  /proc/<name>/status
        processStatusFilePath = malloc(
            strlen(proc) + strlen(procDirContent->d_name) + strlen(status) + 1);
        if (processStatusFilePath == NULL) {
          perror("malloc");
          exit(EXIT_FAILURE);
        }
        processStatusFilePath[strlen(proc) + strlen(procDirContent->d_name)] =
            '\0';
        processStatusFilePath = strcpy(processStatusFilePath, proc);
        processStatusFilePath =
            strcat(processStatusFilePath, procDirContent->d_name);
        processStatusFilePath = strcat(processStatusFilePath, status);

        statusFiled = fopen(processStatusFilePath, "r");
        if (statusFiled == NULL) {
          // we've detected the file but cannot open it, probably because
          // the corresponding process no longer exists
          fprintf(stderr,
                  "file %s detected but cannot opened, probably because the "
                  "corresponding process does not exists\n",
                  processStatusFilePath);

        } else {
          // these three values are defined for each file
          char *CmdNameValue;
          char *CmdUIDValue;
          while (fgets(statusFileLine, STATUS_FILE_LINE_LEN, statusFiled) !=
                 NULL) {
            // Here we are proccessing each file, we should tokenize them by \t
            // character
            char *currentToken;
            currentToken = strtok(statusFileLine, "\t");
            if (strcmp(currentToken, "Name:") == 0) {
              currentToken = strtok(NULL, "\t");
              CmdNameValue = malloc(strlen(currentToken) + 1);
              if (CmdNameValue == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
              }
              CmdNameValue = strcpy(CmdNameValue, currentToken);
              CmdNameValue[strlen(CmdNameValue) - 1] = '\0';
              continue;
            } else if (strcmp(currentToken, "Uid:") == 0) {
              currentToken = strtok(NULL, "\t");
              CmdUIDValue = malloc(strlen(currentToken) + 1);
              if (CmdUIDValue == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
              }
              CmdUIDValue = strcpy(CmdUIDValue, currentToken);
              continue;
            } else {
              continue;
            }
          }
          // check if CmdUID equals user's ID
          // if so then print
          if (strcmp(userIDStr, CmdUIDValue) == 0) {
            printf("Process with name: %s and PID: %s belongs to user %s with "
                   "UID %s\n",
                   CmdNameValue, procDirContent->d_name, argv[1], CmdUIDValue);
          }
          free(CmdNameValue);
          free(CmdUIDValue);
        }

        fclose(statusFiled);
        free(processStatusFilePath);
      }
    }
  }

  closedir(procDIR);

  return 0;
}
