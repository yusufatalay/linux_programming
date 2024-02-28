#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(void)
{
	pid_t childPID, parentPID, grandparentPID;

	setbuf(stdout, NULL);

	grandparentPID = getpid();

	switch (parentPID = fork())
	{
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);

	case 0: // parent
		parentPID = getpid();
		switch (childPID = fork())
		{
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		case 0:	// child (grandchild)
			sleep(10);		// sleep good amount so that parent is gone , and grandparent called wait
			printf("Child is here with PID = %d\n", childPID);
			while (getppid() == parentPID)
			{
				printf("Parent still alive\n");
			}
			printf("Parent gone new parent's PID = %d\n", getppid());
			

		
  		default:
			printf("Parent here with PID = %d about to exit\n", parentPID);
			_exit(EXIT_SUCCESS);
		}

	default: // grandpa
		printf("Grandparent here with PID = %d\n", grandparentPID);
		sleep(3);	// sleep so that parent can schedule first
		wait(NULL);
	}
}