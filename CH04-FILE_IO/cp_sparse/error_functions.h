#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

// errMsg is wrapper for printf with new line at the end.
void errMsg(const char *format, ...);

#ifdef __GNUC__


    /* This macro stops 'gcc -Wall' complaining the "control reaches
       end of non-void function" if we use the following function to
       terminate main() or some other non-void functions.*/

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

// errExit prints error message and exits.
void errExit(const char *format, ...) NORETURN;

// err_exit prints error message and exits
// without flushing stdout before printing err message
void err_exit(const char *format, ...) NORETURN;

// errExitEN same as errExit but with (E)rror(N)umber
void errExitEN(int errnum, const char *format, ...) NORETURN;

void fatal(const char *format, ...) NORETURN;

void usageErr(const char *format, ...) NORETURN;

void cmdLineErr(const char *format, ...) NORETURN;

#endif
