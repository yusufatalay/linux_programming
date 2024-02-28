#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{

	setbuf(stdout, NULL);

	switch (fork())
	{
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);
	case 0:
		printf("Child executing current PPID (parent alive) = %ld\n", (long)getppid());
		sleep(5);
		printf("Parent dead, new parent pid is %ld\n", (long)getppid());
		exit(EXIT_SUCCESS);

	default:
		sleep(3); // Sleep so that child can schedule first
		printf("Parent executing current PID = %ld . I am about to die\n", (long)getpid());
		_exit(EXIT_FAILURE);
	}
}