#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void){
	pid_t childPid = fork();

	switch (childPid) {
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		case 0:		// Child
			sleep(10);	// Make sure the parent exits
			printf("Child: parent exited, will try to read from stdin\n");
			char *buf[10];
			
			if (read(STDIN_FILENO, buf, 9 ) == -1){
				perror("read");		// read: Input/output error 
				exit(EXIT_FAILURE);
			}

			exit(EXIT_SUCCESS);
		default:	// Parent
			// exit immediately for orphaneing the pg
			printf("Parent:  exiting now\n");
			_exit(0);
	}
}
