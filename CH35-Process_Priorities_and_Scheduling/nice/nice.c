#define _GNU_SOURCE
#include <sched.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int nice;
	// if no argument has provided then print out current nice value
	if (argc == 1)
	{
		nice = getpriority(PRIO_PROCESS, 0);

		printf("%d\n", nice);

		return 0;
	}

	int priority = 10; // default priority value is 10

	int helpFlag = 0;
	int priorityFlag = 0;
	char *priorityValue = NULL;
	int index;
	int c;

	opterr = 0;

	while ((c = getopt(argc, argv, "n:")) != -1)
	{
		switch (c)
		{
		case 'n':
			priorityFlag = 1;
			priorityValue = optarg;
			break;
		default:
			helpFlag = 1;
			break;
		}
	}

	if (argc < 2 || helpFlag)
	{
		printf("Usage: nice [OPTION] [COMMAND [ARG]...]\n");
		return 0;
	}

	// arrange the command name and args
	char *command;
	char **args;
	int i; // i will be the index for the args
	// convert priority if -n flag is present
	// if -n flag exists then the command name at 3rd index else its the index 1

	if (priorityFlag)
	{
		command = strdup(argv[3]);
		if (command == NULL)
		{
			perror("strdup");
			return 125;
		}

		args = malloc((argc - 2) * sizeof(char *));
		if (args == NULL)
		{
			perror("malloc");
			return 125;
		}

		for (i = 3; i < argc; i++)
		{
			args[i - 3] = strdup(argv[i]);
			if (args[i - 3] == NULL)
			{
				perror("strdup");
				return 125;
			}
		}
		args[argc - 3] = NULL; // Null terminate the array of arguments

		priority = strtol(priorityValue, NULL, 10);
		if (priority == EINVAL)
		{
			perror("strtol");
			return 125;
		}
	}
	else
	{
		command = strdup(argv[1]);
		if (command == NULL)
		{
			perror("strdup");
			return 125;
		}

		args = malloc((argc) * sizeof(char *));
		if (args == NULL)
		{
			perror("malloc");
			return 125;
		}

		for (i = 1; i < argc; i++)
		{
			args[i - 1] = strdup(argv[i]);
			if (args[i - 1] == NULL)
			{
				perror("strdup");
				return 125;
			}
		}
		args[argc - 1] = NULL; // Make the args end with NULL for execv
	}

	// set the priority
	if (setpriority(PRIO_PROCESS, 0, priority) == -1)
	{
		perror("setpriority");
		return 125;
	}

	// call execv
	execvp(command, args);
	perror("execv");
	return 125;
}