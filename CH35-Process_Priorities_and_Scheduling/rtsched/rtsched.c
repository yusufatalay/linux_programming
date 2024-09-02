#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc < 4 || strchr("fr", argv[1][0]) == NULL)
	{
		printf("Usage: %s policy (r|f) priority command arg...", argv[0]);
		exit(EXIT_FAILURE);
	}

	int policy;
	int priority;
	char *command;
	char **args;
	struct sched_param sp;

	// Get policy
	policy =  strcmp(argv[1], "r") == 0 ? SCHED_RR : SCHED_FIFO;

 	// get priority
	priority = strtol(argv[2], NULL, 10);
	if (priority == EINVAL)
	{
		perror("strtol");
		exit(EXIT_FAILURE);
	}

	// set priority
	sp.sched_priority = priority;

	command = strdup(argv[3]);
	if (command == NULL)
	{
		perror("strdup");
		exit(EXIT_FAILURE);
	}

	// re-arrange argv so that it points to the user supplied arguments to the command
	argv = &argv[3];

	// change current process' scheduling policy and priority
	if (sched_setscheduler(getpid(), policy, &sp) == -1){
		perror("sched_setscheduler");
		exit(EXIT_FAILURE);
	}


	// execute the command with args
	execvp(command, argv );
	perror("execve");

}
