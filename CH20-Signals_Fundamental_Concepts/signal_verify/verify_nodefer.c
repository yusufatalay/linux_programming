#define _GNU_SOURCE
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

static void handler(int signum)
{
	printf("signal handled\n");
	sleep(10);
}

int main(void)
{
	struct sigaction action;
	action.sa_handler = &handler;
	sigemptyset(&action.sa_mask);
	 action.sa_flags = SA_NODEFER;

	sigaction(SIGINT, &action, NULL);
	for (;;)
	{
		pause();
	}

	exit(EXIT_SUCCESS);
}