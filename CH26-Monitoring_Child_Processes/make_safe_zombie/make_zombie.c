#define _GNU_SOURCE
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

#define CMD_SIZE 200

int main(int argc, char *argv[])
{
	char cmd[CMD_SIZE];
	pid_t childPid;
	siginfo_t childInfo;

	setbuf(stdout, NULL);

	printf("Parent PID=%ld\n", (long)getpid());

	switch (childPid = fork())
	{
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);

	case 0: // child immediately exits to become a zombie
		printf("Child (PID=%ld) exiting\n", (long)getpid());
		_exit(EXIT_SUCCESS);

	default: // Parent
		//
		memset(&childInfo, 0, sizeof(siginfo_t));
		
		// instead of using signals, I've used waitid with WNOWAIT for synchronisation
		 if (waitid(P_ALL, -1, &childInfo, WEXITED | WSTOPPED  | WCONTINUED | WNOWAIT) == -1)
		{
			perror("waitid()");
			exit(EXIT_FAILURE);
		}
		snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
		system(cmd);

		// Now send the "sure kill" signal to the zombie
		if (kill(childPid, SIGKILL) == -1)
		{
			perror("kill");
		}
		sleep(3); // Give child a chance to react to signal
		printf("After sending SIGKILL to zombie (PID=%ld):\n", (long)childPid);
		// view zombie child again
		system(cmd);
		exit(EXIT_SUCCESS);
	}
}