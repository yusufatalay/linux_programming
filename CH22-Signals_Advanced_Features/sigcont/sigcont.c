#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void sigcontHandler(int signum)
{
	// UNSAFE: this handler uses non-async-signal-safe functions.
	fflush(0);
	printf("SIGCONT IS HANDLED\n");
}

int main(void)
{
	// set the signal handler
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = &sigcontHandler;

	if (sigaction(SIGCONT, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	// block SIGCONT
	sigset_t blocked_sig;
	if (sigemptyset(&blocked_sig) == -1)
	{
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}

	if (sigaddset(&blocked_sig, SIGCONT) == -1)
	{
		perror("sigaddset");
		exit(EXIT_FAILURE);
	}

	if (sigprocmask(SIG_BLOCK, &blocked_sig, NULL) == -1)
	{
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	fflush(0);
	printf("SIGCONT is now blocked\n");
	// Here the SIGCONT is blocked therefore we cannot see the handler's message when we intercept
	// the program with Control-Z and  resume with fg

	sleep(30);

	if (sigprocmask(SIG_UNBLOCK, &blocked_sig, NULL) == -1)
	{
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	printf("SIGCONT is now unblocked handler invoked before this text\n");

	exit(EXIT_SUCCESS);
}