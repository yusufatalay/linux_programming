#define _GNU_SOURCE
#include <stdlib.h>
#include "signal_functions.h"

static int sigCnt[NSIG]; // Counts deliveries of each signal

static volatile sig_atomic_t gotSigint; // Set nonzero if SIGINT is delivered

static void handler(int sig)
{
	if (sig == SIGINT)
	{
		gotSigint = 1;
	}
	else
	{
		sigCnt[sig]++;
	}
}

int main(int argc, char *argv[])
{
	int n, numSecs;
	sigset_t pendingMask, blockingMask, emptyMask;
	struct sigaction newaction;

	newaction.sa_handler = handler;
	sigemptyset(&newaction.sa_mask);

	printf("%s: PID is %ld\n", argv[0], (long)getpid());

	for (n = 1; n < NSIG; n++)
	{
		// (void)signal(n, handler);  the challenge was to convert this to sigaction call
		if (sigaction(n, &newaction, NULL) == -1)
		{
			perror("sigaction");
			exit(EXIT_FAILURE);
		}
	}

	/* If a sleep time was specified, temporarily block all signals,
	sleep (while another process sends us signals), and then display
	the mask of pending signals and unblock all signals.*/

	if (argc > 1)
	{
		numSecs = atoi(argv[1]); // TODO: change this to stol

		sigfillset(&blockingMask);
		if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1)
		{
			perror("sigprocmask");
			exit(EXIT_FAILURE);
		}

		printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
		sleep(numSecs);

		if (sigpending(&pendingMask) == -1)
		{
			perror("sigpending");
			exit(EXIT_FAILURE);
		}

		printf("%s: pending signals are: \n", argv[0]);
		printSigset(stdout, "\t\t", &pendingMask);

		sigemptyset(&emptyMask);
		if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1)
		{
			perror("sigprocmask");
			exit(EXIT_FAILURE);
		}
	}

	while (!gotSigint)
	{
		continue;
	}

	for (n = 1; n < NSIG; n++)
	{
		if (sigCnt[n] != 0)
		{
			printf("%s: signal %d caught %d time%s\n", argv[0], n, sigCnt[n],
			       (sigCnt[n] == 1) ? "" : "s");
		}
	}

	exit(EXIT_SUCCESS);
}
