#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/wait.h>


static long getTick(struct tms *t);
void job(const char *doer);

int main(void){
	cpu_set_t set;
	struct sched_param sp;
	struct tms tData;
	int childStatus;
	pid_t child;

	// set cpu affinity to CPU1 so that child and parent would run on the same one (child inherits the affinity).
	CPU_ZERO(&set);
	CPU_SET(0, &set);
	if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1){
		perror("sched_setaffinity");
		exit(EXIT_FAILURE);
	}

	// set the scheduling policy
	sp.sched_priority = sched_get_priority_min(SCHED_FIFO);

	if (sched_setscheduler(getpid(), SCHED_FIFO, &sp  ) ==-1){
		perror("sched_setscheduler");
		exit(EXIT_FAILURE);
	}

	// create a child
	if ((child = fork()) == 0){
		// child process
		job("child");
	}else{
		job("parent");
	}

	// wait child
	wait(&childStatus);

	exit(EXIT_SUCCESS);
}


static long getTick(struct tms *t){
	return t->tms_utime;
}

void job(const char *doer){
	struct tms tData;
 	int freq=sysconf(_SC_CLK_TCK);
	int totalQuarters = 12;	// 3*4 =12 quarters in 3 seconds
	int currentQuarter = 0;
	
	while(1){
		if(times(&tData) == (clock_t) -1){
			perror("times");
			exit(EXIT_FAILURE);
		}

		// is it quarter
		if(getTick(&tData) % (long)((0.25)*freq) == 0){
			printf("%s a quarter of second passed\n", doer);
			currentQuarter++;

		// a second passed?
			if (currentQuarter %4 == 0){
				printf("%s yielding\n", doer);
				if (sched_yield() == -1){
					perror("sched_yield");
					exit(EXIT_FAILURE);
				}
			}

			if (currentQuarter == totalQuarters){
				printf("%s DONE\n",doer);
				break;
			}
		}
	}
}