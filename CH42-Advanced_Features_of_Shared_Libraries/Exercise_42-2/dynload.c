#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

int main(int argc, char *argv[])
{
  void *libHandle;     // Handle for shared library
  void (*funcp)(void); // Pointer to function with no arguments
  const char *err;
  Dl_info dlinfo;

  if (argc != 3 || strcmp(argv[1], "--help") == 0)
  {
    printf("Usage %s lib-path func-name\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Load the shared library and get a handle for later use

  libHandle = dlopen(argv[1], RTLD_LAZY);
  if (libHandle == NULL)
  {
    fprintf(stderr, "dlopen: %s", dlerror());
    exit(EXIT_FAILURE);
  }

  // search library for symbol named in argv[2]

  (void)dlerror(); // clear dlerror()
  *(void **)(&funcp) = dlsym(libHandle, argv[2]);
  err = dlerror();
  if (err != NULL)
  {
    fprintf(stderr, "dlsym: %s", err);
    exit(EXIT_FAILURE);
  }

  if (dladdr(libHandle, &dlinfo) == 0)
  {
    fprintf(stderr, "dladdr: %s", err);
    exit(EXIT_FAILURE);
  }

  printf("Library %s at address %p\nSymbol %s at address %p\n",
         dlinfo.dli_fname, dlinfo.dli_fbase, dlinfo.dli_sname, dlinfo.dli_saddr);

  // Try calling the address returned by dlsym() as a function
  (*funcp)();

  dlclose(libHandle); // close the lib

  exit(EXIT_SUCCESS);
}
