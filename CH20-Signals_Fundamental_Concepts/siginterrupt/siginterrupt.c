#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
int my_siginterrupt(int sig, int flag)
{
	if ((flag != 0) && (flag != 1))
	{
		return -1;
	}
	// retrieve signal disposition;
	struct sigaction sa;

	if (sigaction(sig, NULL, &sa) == -1)
	{
		return -1;
	}

	if (flag)
	{
		sa.sa_flags &= ~SA_RESTART;
	}
	else
	{
		sa.sa_flags |= SA_RESTART;
	}

	if (sigaction(sig, &sa, NULL) == -1)
	{
		return -1;
	}

	return 0;
}