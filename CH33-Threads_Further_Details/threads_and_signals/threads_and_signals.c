#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

void printfSigset(FILE *of, const char *prefix, const sigset_t *sigset);
int printPendingSigs(FILE *of, const char *msg);

void *threadFunc(void *arg)
{

	sigset_t block;
	sigemptyset(&block);
	sigaddset(&block, SIGUSR1);
	sigaddset(&block, SIGUSR2);

	if (sigprocmask(SIG_BLOCK, &block, NULL) != 0)
	{
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	pthread_t self;
	self = pthread_self();

	
	char msg[200];
	sprintf(msg, "At thead %lu, its pending signals are:\n", self);

	sleep(10); // let the signals be sent

	printPendingSigs(stdout, msg);

	return NULL;
}

int main(void)
{
	pthread_t t1, t2;

	pthread_create(&t1, NULL, threadFunc, NULL);
	pthread_create(&t2, NULL, threadFunc, NULL);

	sleep(1); // let the threads block signals

	pthread_kill(t1, SIGUSR1);
	pthread_kill(t2, SIGUSR2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	exit(EXIT_SUCCESS);
}

void printfSigset(FILE *of, const char *prefix, const sigset_t *sigset)
{
    int sig, cnt;

    cnt = 0;
    for (sig = 1; sig < NSIG; sig++)
    {
        if (sigismember(sigset, sig))
        {
            cnt++;
            fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
        }
    }

    if (cnt == 0)
    {
        fprintf(of, "%s<empty signal set>\n", prefix);
    }
}

int printPendingSigs(FILE *of, const char *msg)
{
	sigset_t pendingSigs;

	if (msg != NULL)
	{
		fprintf(of, "%s", msg);
	}

	if (sigpending(&pendingSigs) == -1)
	{
		return -1;
	}

	printfSigset(of, "\t\t", &pendingSigs);

	return 0;
}
