#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>

void sigHandler(int signum)
{
	printf("At signal handler with signal : %s\n", strsignal(signum));
}

int main(void)
{
	// Sender will send the receiver two signals (SIGTTIN, SIGTSTP),
	// the SIGTSTP is SIG_DFL and SIGTTIN has a handler registered.
	printf("PGRPID: %ld\n", (long)getpgrp());
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigHandler;
	if (sigaction(SIGTTIN, &sa, NULL) != 0)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	pid_t childPid;

	childPid = fork();

	switch (childPid)
	{
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);
	case 0: // signalSender

		printf("Child[%ld]: sleeping for 10 secs\n", (long)getpid());
		sleep(10); // wait for receiver to born and parent to exit
		printf("Child[%ld]: sleeping done sending signal to PGRPID %ld\n", (long)getpid, (long)getpgrp());

		if (kill(getpid(), SIGTTIN) == -1)
		{
			perror("kill");
			exit(EXIT_FAILURE);
		}
		if (kill(getpid(), SIGTSTP) == -1)
		{
			perror("kill");
			exit(EXIT_FAILURE);
		}
		printf("Child[%ld]: Signals are sent waiting\n", (long)getpid());
		for (;;)
		{
			pause();
		}
	default: // Parent -> exit so that the process group becomes orphan
		printf("Parent[%ld]: exiting now\n", (long)getpid());
		_exit(EXIT_SUCCESS);
	}
}