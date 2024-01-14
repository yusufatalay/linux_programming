#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Explanation for the SA_RESETHAND in the code: when first interrupt occurs the "handler" function is called
after that program returns to the main block, and when the second interrupt occurs program terminates with SIG_INT
thats because we've initialized the sigaction struct with SA_RESETHAND (reset handler after first use).
*/
static void handler(int signum)
{
	printf("catched signal in handler %d : %s\n", signum, strsignal(signum));
}

int main(void)
{
	struct sigaction action;
	action.sa_handler = &handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESETHAND;

	// assign interrrupt (2) signal to this handler;
	sigaction(2, &action, NULL);

	// wait for a signal
	for (;;)
	{
		pause();
	}

	exit(EXIT_SUCCESS);
}