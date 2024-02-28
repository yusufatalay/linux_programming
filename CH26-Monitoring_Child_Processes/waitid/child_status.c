#define _GNU_SOURCE
#include <sys/wait.h>
#include "tlpi_hdr.h"

void printChildInfo(const siginfo_t childinfo);

int main(int argc, char *argv[])
{
	pid_t childPid;
	siginfo_t childInfo;

	if (argc > 1 && strcmp(argv[1], "--help") == 0)
	{
		printf("%s [exit-status]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	switch (fork())
	{
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);
	case 0: /*Child: either exits immediately with given
		status or loops waiting for signals*/
		printf("Child started with PID = %ld\n", (long)getpid());
		if (argc > 1)
		{
			exit(getInt(argv[1], 0, "exit-status"));
		}
		else
		{
			for (;;)
			{
				pause();
			}
			exit(EXIT_FAILURE); // won't reach here but good practice
		}
	default: /*Parent: repeatedly wait on child until it
		 either exits or si terminated by a signal*/
		for (;;)
		{

			// waitid
			// zero-out the child
			memset(&childInfo, 0, sizeof(siginfo_t));
			// since we are using P_ALL the id parameter is ignored
			if (waitid(P_ALL, -1, &childInfo, WEXITED | WSTOPPED | WCONTINUED) == -1)
			{
				perror("waitid");
				exit(EXIT_FAILURE);
			}

			// Print status in hex, and as seperate decimal bytes
			//
			printf("waitid() returned: PID=%ld; status=0x%04x (%d,%d)\n",
			       (long)childInfo.si_pid, (unsigned int)childInfo.si_status, childInfo.si_status >> 8, childInfo.si_status & 0xff);

			printChildInfo(childInfo);

			if (WIFEXITED(childInfo.si_status) || WIFSIGNALED(childInfo.si_status))
			{
				exit(EXIT_SUCCESS);
			}
		}
	}
}

void printChildInfo(const siginfo_t childInfo)
{

	printf("Information gathered from si_code\n------------------------------:\n");
	switch (childInfo.si_code)
	{
	case CLD_EXITED:
		printf("Child terminated by calling _exit\n");
		break;

	case CLD_KILLED:
		printf("Child killed by a signal\n");
		break;

	case CLD_STOPPED:
		printf("Child stopped by a signal\n");
		break;

	case CLD_CONTINUED:
		printf("Child continued by a signal\n");
		break;
	}

	printf("\n\nInformation gathered from si_status\n----------------------------:\n");
	if (WIFEXITED(childInfo.si_status))
	{
		printf("child exited, status=%d\n", WEXITSTATUS(childInfo.si_status));
	}
	else if (WIFSIGNALED(childInfo.si_status))
	{
		printf("child is killed by signal %d (%s)", WTERMSIG(childInfo.si_status), strsignal(WTERMSIG(childInfo.si_status)));
#ifdef WCOREDUMP // Not in SUSv3, may be absent on some systems
		if (WCOREDUMP(childInfo.si_status))
		{
			printf("(core dumped)");
		}
#endif
		printf("\n");
	}
	else if (WIFSTOPPED(childInfo.si_status))
	{
		printf("child is stopped by signal %d (%s)\n", WSTOPSIG(childInfo.si_status), strsignal(WSTOPSIG(childInfo.si_status)));
#ifdef WIFCONTINUED // SUSv3 has this, but older Linux versions and
		    // some other UNIX implementations don't
	}
	else if (WIFCONTINUED(childInfo.si_status))
	{
		printf("child is continued\n");
#endif
	}
	else
	{ // Should never happen
		printf("what happened to this child? (status=%x)\n", (unsigned int)childInfo.si_status);
	}
}