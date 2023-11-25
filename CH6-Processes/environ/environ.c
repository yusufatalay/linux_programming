#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int my_setenv(const char *name, const char *value, int overwrite) {
  if (name == NULL) {
    errno = EINVAL;
    return -1; // indicate failure
  }

  // prepare key-value pair for putenv
  // allocate enough space for pair
  char *pair = (char *)malloc(strlen(name) + strlen(value) +
                              2); // +2 for '=' and null terminator
  if (pair == NULL) {
    errno = ENOMEM;
    return -1; // indicate failure
  }

  strcat(strcat(strcpy(pair, name), "="), value);

  // if overwrite is set then just use putenv without checking the
  // existence of the key
  // check whether the name exists in the env,
  // if so then return success, if not then set it

  if (overwrite || getenv(name) != NULL) {
    if (putenv(pair) != 0) {
      free(pair); // free allocated memory
      return -1;  // indicate failure
    }
  }

  free(pair); // free allocated memory
  return 0;   // indicate success
}

int my_unsetenv(const char *name) {
  // to unset an environment variable
  // we just need to set its value to an empty string

  // +2 because 1 for '=' and 1 for null termination
  char *res = (char *)malloc(strlen(name) + 2);
  if (res == NULL) {
    errno = ENOMEM;
    return -1; // indicate failure
  }

  strcat(strcpy(res, name), "=");

  if (putenv(res) != 0) {
    free(res); // free allocated memory
    return -1; // indicate failure
  }

  free(res); // free allocated memory
  return 0;  // indicate success
}

int main() {
  // Test my_setenv
  printf("Setting MY_VARIABLE to my_value: %s\n",
         my_setenv("MY_VARIABLE", "my_value", 1) == 0 ? "Success" : "Failure");
  printf("Setting MY_VARIABLE to new_value: %s\n",
         my_setenv("MY_VARIABLE", "new_value", 0) == 0 ? "Success" : "Failure");

  // Test my_unsetenv
  printf("Unsetting MY_VARIABLE: %s\n",
         my_unsetenv("MY_VARIABLE") == 0 ? "Success" : "Failure");
  printf("Unsetting NON_EXISTENT_VARIABLE: %s\n",
         my_unsetenv("NON_EXISTENT_VARIABLE") == 0 ? "Success" : "Failure");

  // Print the current environment to verify changes
  char *env_var = getenv("MY_VARIABLE");
  if (env_var != NULL) {
    printf("MY_VARIABLE is set to: %s\n", env_var);
  } else {
    printf("MY_VARIABLE is not set.\n");
  }

  return 0;
}
