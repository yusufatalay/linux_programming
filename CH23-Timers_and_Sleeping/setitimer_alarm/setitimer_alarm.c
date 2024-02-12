#include <bits/time.h>
#include <stdio.h>
#include <sys/time.h>

/*setitimer_alarm is implementation for alarm() that accepts and returns unsigned ,setitimer() accepts long returns -1on error*/
long setitimer_alarm(long seconds)
{

	struct itimerval new, old;

	new.it_value.tv_sec = (long)(seconds);
	new.it_value.tv_usec = 0;
	new.it_interval.tv_sec = 0;
	new.it_interval.tv_usec = 0;

	if (setitimer(ITIMER_REAL, &new, &old) == -1)
	{
		return -1;
	}

	return old.it_value.tv_sec;
}

int main()
{

	long old = setitimer_alarm(5);
	if (old == -1)
	{
		perror("setitimer_alarm failed");
		return 1;
	}

	printf("previous alarm value %ld\n", old);

	old = setitimer_alarm(8);
	if (old == -1)
	{
		perror("setitimer_alarm failed");
		return 1;
	}

	printf("previous alarm value %ld\n", old);

	// disable alarm
	old = setitimer_alarm(0);
	if (old == -1)
	{
		perror("setitimer_alarm failed");
		return 1;
	}

	printf("previous alarm value %ld\n", old);

	while (1)
	{
		// Do nothing, just wait for the alarm to go off
	}

	return 0;
}