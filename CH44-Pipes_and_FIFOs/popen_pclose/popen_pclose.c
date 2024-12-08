#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define READ 'r'
#define WRITE 'w'

// map[fileDesc]: childPID 
int FDPIDMAP[1024];

FILE *my_popen(const char *command, const char *type){
	int pfd[2];
	int closeFD;
	int saveFD;
	pid_t childPID;
	if (!((*type != READ) || (*type != WRITE))){
		errno = -EINVAL;
		return NULL;
	}
	// pipe and fork
	if (pipe(pfd) == -1){
		return NULL;
	}
	switch (childPID = fork())
	{
	case -1 :
	 	return NULL;
	case 0: 
		break;
	default:
		// in parent process save the childPID and fileNo in a map
		// if type is read then close the write end of the pipe
		closeFD = (*type == READ) ? pfd[1] : pfd[0];
		saveFD = (*type == READ) ? pfd[0] : pfd[1];

		if (close(closeFD) == -1){
			return NULL;
		}

		FDPIDMAP[saveFD] = childPID;
		return fdopen(saveFD, type);
	}

	// child
	// depending on the type close the un-used end of the pipe
	// re-direct the type to stdin or stdout
	// let the child process run the command 	
	switch (*type) {
	case READ:
		if (close(pfd[0]) == -1)
			return NULL;

		if (pfd[1] != STDOUT_FILENO) {
			if (dup2(pfd[1], STDOUT_FILENO) == -1)
				return NULL;

			if (close(pfd[1]) == -1)
				return NULL;
		}

		break;

	case WRITE:
		if (close(pfd[1]) == -1)
			return NULL;

		if (pfd[0] != STDIN_FILENO) {
			if (dup2(pfd[0], STDIN_FILENO) == -1)
				return NULL;

			if (close(pfd[0]) == -1)
				return NULL;
		}
	}

	execlp("/bin/sh", "/bin/sh", "-c", command, (char *) 0);

	return NULL;
}

int my_pclose(FILE *stream){
	// find the child pid from fileno(stream)
	int fd = fileno(stream);
	if (fd == -1){
		return -1;
	}

	pid_t childPid = FDPIDMAP[fd];

	if (childPid > 0){
		int status; 
		if (waitpid(childPid, &status, 0) == -1){
			return -1;
		}
		return WEXITSTATUS(status);

	}else{
		return -1;
	}
	
	// wait for it
}