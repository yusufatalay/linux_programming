#define _GNU_SOURCE
#include <stdlib.h>
#include <signal.h>

int main(void)
{
	struct sigaction action;
	action.sa_handler = SIG_IGN;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	for (int i = 1; i < NSIG; i++)
	{
		sigaction(i, &action, NULL);
	}

	pause();

	exit(EXIT_SUCCESS);
}