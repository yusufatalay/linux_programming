#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>

struct passwd *my_getpwnam(const char *name) {
  struct passwd *result;

  while ((result = getpwent()) != NULL) {
    if (strcmp(result->pw_name, name) == 0) {
      // Close the file and return the result
      endpwent();
      return result;
    }
  }

  // If we reach here, the user entry was not found
  endpwent();
  return NULL;
}
