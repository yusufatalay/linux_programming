#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
	pid_t childPid;
	struct rusage usage;


	switch (childPid = fork())
	{
	case -1:
	 	perror("fork");
		exit(EXIT_FAILURE);
	case 0:
		// child : consume some CPU time then _exit
		struct tms t;
		while(1){
			if(times(&t) == (clock_t)-1){
				perror("times");
				_exit(EXIT_FAILURE);
			}

			// consume 3 seconds
			if((t.tms_utime / sysconf(_SC_CLK_TCK)) >= 3){
				_exit(EXIT_SUCCESS);
			}
		}
		
	default:
		// parent
		// do some cpu time consuming
		// get rusage
		struct tms tp;
		while((tp.tms_utime / sysconf(_SC_CLK_TCK)) < 2){
			if(times(&tp) == (clock_t)-1){
				perror("times");
				exit(EXIT_FAILURE);
			}
		}

		printf("Parent waited for %ld seconds\n",tp.tms_utime / sysconf(_SC_CLK_TCK));

		
		if (getrusage(RUSAGE_CHILDREN, &usage) == -1){
			perror("getrusage");
			exit(EXIT_FAILURE);
		}
		
		printf("From parent for child: system user time BEFORE wait: %ld\n", usage.ru_utime.tv_sec);

		int childStatus;	
		wait(&childStatus);

		if (getrusage(RUSAGE_CHILDREN, &usage) == -1){
			perror("getrusage");
			exit(EXIT_FAILURE);
		}
		
		printf("From parent for child: system user time AFTER wait: %ld\n", usage.ru_utime.tv_sec);
		exit(EXIT_SUCCESS);
	}
}
