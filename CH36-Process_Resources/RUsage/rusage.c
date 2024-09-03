#include <bits/types/struct_rusage.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    pid_t childPid;
    struct rusage ru;
    int childStatus;
    char *command = strdup(argv[1]);

    if (argc < 2){
        printf("Usage %s command arg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    switch (childPid = fork())
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        // child runs the program
        // rearrange argv 
        argv = &argv[1];
        execvp(command, argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    default:
        // parent observes the child
        if (getrusage(RUSAGE_CHILDREN, &ru) == -1){
            perror("getrusage");
            exit(EXIT_FAILURE);
        }

	if(wait(&childStatus) == -1){
		perror("wait");
		exit(EXIT_FAILURE);
	}


        if (getrusage(RUSAGE_CHILDREN, &ru) == -1){
            perror("getrusage");
            exit(EXIT_FAILURE);
        }

        printf("User time: %ld.%06ld secs\n", ru.ru_utime.tv_sec, ru.ru_utime.tv_usec);
        printf("System time: %ld.%06ld secs\n", ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
        printf("Executed in %ld.%06ld secs\n", 
               ru.ru_utime.tv_sec + ru.ru_stime.tv_sec, 
               ru.ru_utime.tv_usec + ru.ru_stime.tv_usec);

        exit(EXIT_SUCCESS);
    }

    // code should never come here    
    exit(EXIT_SUCCESS);
}