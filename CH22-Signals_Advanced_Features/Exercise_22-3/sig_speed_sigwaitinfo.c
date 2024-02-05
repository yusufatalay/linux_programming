#define _GNU_SOURCE
#include <signal.h>
#include "tlpi_hdr.h"


#define TESTSIG SIGUSR1

void handler(int n){

}
int
main(int argc, char *argv[])
{
    int numSigs, scnt;
    pid_t childPid;
    sigset_t blockedMask, emptyMask;
    struct sigaction sa;
    siginfo_t si;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s num-sigs\n", argv[0]);

    numSigs = getInt(argv[1], GN_GT_0, "num-sigs");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(TESTSIG, &sa, NULL) == -1)
        errExit("sigaction");

    /* Block the signal before fork(), so that the child doesn't manage
       to send it to the parent before the parent is ready to catch it */

    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1)
        errExit("sigprocmask");

    sigemptyset(&emptyMask);

    switch (childPid = fork()) {
    case -1: errExit("fork");

    case 0:     /* child */
        for (scnt = 0; scnt < numSigs; scnt++) {
            if (kill(getppid(), TESTSIG) == -1)
                errExit("kill");
            if (sigwaitinfo(&emptyMask, &si) == -1 && errno != EINTR) // we're ignoring the information of the catched signal
                    errExit("sigsuspend");
        }
        exit(EXIT_SUCCESS);

    default: /* parent */
        for (scnt = 0; scnt < numSigs; scnt++) {
            if (sigwaitinfo(&emptyMask, &si) == -1 && errno != EINTR)// we're ignoring the information of the catched signal
                    errExit("sigsuspend");
            if (kill(childPid, TESTSIG) == -1)
                errExit("kill");
        }
        exit(EXIT_SUCCESS);
    }
}
