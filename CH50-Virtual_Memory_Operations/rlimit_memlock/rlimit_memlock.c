#include <stdio.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
	// get RLIMIT_MEMLOCK value
	struct rlimit lim;
	if (getrlimit(RLIMIT_MEMLOCK, &lim) == -1){
		perror("getrlimit");
		exit(EXIT_FAILURE);
	}
	printf("Current RLIMIT_MEMLOCK %lu\n",lim.rlim_cur );
	printf("Current Maximum RLIMIT_MEMLOCK %lu\n",lim.rlim_max );


	// set rlimit_memlock value to 1 page size
	long pageSize = sysconf(_SC_PAGESIZE);
	if (pageSize == -1){
		perror("sysconf");
		exit(EXIT_FAILURE);
	}

	lim.rlim_cur = (unsigned long) pageSize;
	lim.rlim_max = (unsigned long) pageSize;

	if(setrlimit(RLIMIT_MEMLOCK, &lim) == -1){
		perror("setrlimit");
		exit(EXIT_FAILURE);
	}

	// request memlock for 2 times of the pagesize
	char *addr;
	addr = mmap(NULL, sizeof(addr), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
	if (addr == NULL){
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	if (mlock(addr, 2 * pageSize) == -1){
		perror("mlock");
		exit(EXIT_FAILURE);
	}
	//  Output: mlock: Cannot allocate memory
}