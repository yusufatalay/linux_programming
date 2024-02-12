#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	timer_t nullEventTimerID;
	struct itimerspec nullEventTimerSpec;
	if (timer_create(CLOCK_REALTIME, NULL, &nullEventTimerID) != 0)
	{
		perror("timer_create");
		exit(EXIT_FAILURE);
	}
	nullEventTimerSpec.it_value.tv_sec = 5;
	nullEventTimerSpec.it_value.tv_nsec = 0;
	nullEventTimerSpec.it_interval.tv_sec = 0;
	nullEventTimerSpec.it_interval.tv_nsec = 0;

	timer_t timerWithEventID;
	struct itimerspec timerWithEventSpec;
	struct sigevent timerEvent;
	timerEvent.sigev_notify = SIGEV_SIGNAL;
	timerEvent.sigev_signo = SIGALRM;
	timerEvent.sigev_value.sival_int = (int)timerWithEventID;

	if (timer_create(CLOCK_REALTIME, &timerEvent, &timerWithEventID) != 0)
	{
		perror("timer_create");
		exit(EXIT_FAILURE);
	}
	timerWithEventSpec.it_value.tv_sec = 5;
	timerWithEventSpec.it_value.tv_nsec = 0;
	timerWithEventSpec.it_interval.tv_sec = 0;
	timerWithEventSpec.it_interval.tv_nsec = 0;

	// arm both timers in same manner
	// both timers yields same output, uncomment them each one for a run to try

	// if (timer_settime(nullEventTimerID, 0, &nullEventTimerSpec, NULL) != 0)
	//{
	//	perror("timer_settime");
	//	exit(EXIT_FAILURE);
	//}

	// if (timer_settime(timerWithEventID, 0, &timerWithEventSpec, NULL) != 0)
	//{
	//	perror("timer_settime");
	//	exit(EXIT_FAILURE);
	// }

	// Wait for timer to expire
	for (;;)
		;
}