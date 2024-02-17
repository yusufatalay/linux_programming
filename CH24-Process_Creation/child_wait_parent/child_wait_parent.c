#define _GNU_SOURCE
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define SYNC_CSIG SIGUSR1 // Synchronization signal from child to parent 
#define SYNC_PSIG SIGUSR2 // Synchronization signal from  parent to child 

static void handler(int sig)
{
	printf("From handler: ");
	switch (sig){
		case SYNC_CSIG:
			printf("Child send signal to parent\n");
			break;
		case SYNC_PSIG:
			printf("Parent send signal to child\n");
			break;
	}
}

int main(void)
{
	pid_t childPid;
	sigset_t blockMask, origMask, emptyMask;
	struct sigaction sa;

	setbuf(stdout, NULL); // Disable buffering for stdout
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SYNC_CSIG);
	sigaddset(&blockMask, SYNC_PSIG);
	if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1)
	{
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;

	if (sigaction(SYNC_CSIG, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	if (sigaction(SYNC_PSIG, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	switch (childPid = fork())
	{
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);

	case 0:
		printf("Child started - doing some work\n");
		sleep(2);
		printf("Child about to signal parent\n");
		if (kill(getppid(), SYNC_CSIG) == -1)
		{
			perror("kill");
			exit(EXIT_FAILURE);
		}
		printf("child about to wait for signal\n");
		sigemptyset(&emptyMask);
		if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
		{
			perror("sigsuspend");
			exit(EXIT_FAILURE);
		}
		printf("child got signal\n");

		if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
		{
			perror("sigprocmask");
			exit(EXIT_FAILURE);
		}


		_exit(EXIT_SUCCESS);
	default:
		printf("Parent about to wait for signal\n");
		sigemptyset(&emptyMask);
		if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
		{
			perror("sigsuspend");
			exit(EXIT_FAILURE);
		}
		printf("Parent got signal\n");

		if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
		{
			perror("sigprocmask");
			exit(EXIT_FAILURE);
		}
		
		printf("Parent doing some work\n");
		sleep(2);
		printf("Parent about to signal parent\n");
		if (kill(getppid(), SYNC_PSIG) == -1)
		{
			perror("kill");
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
	}
}
