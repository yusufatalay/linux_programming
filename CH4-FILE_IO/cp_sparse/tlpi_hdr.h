#ifndef TLPI_HDR_H
#define TLPI_HDR_H               // Prevent accidential double inclusion

#include <sys/types.h>           // Type definitions used by many programs
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>              // Prototypes for many system calls
#include <errno.h>
#include <string.h>

#include "error_functions.h"     // Declares our error-handling functions

typedef enum {FALSE, TRUE} Boolean;

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n))

#endif
