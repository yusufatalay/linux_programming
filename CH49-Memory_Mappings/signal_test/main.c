#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>


int main(void){
	/* To get SIGSEGV 
	* try to reference beyond mapped range
	*/

	/* To get SIGBUS
	* try to reference a point that is in the mapped range but exceeds file's size
	*/

	// my system's pagesize is 4096 
	// getconf PAGE_SIZE

	int pagesize = 4096; 

	// create a file that is half of the size of pagesize
	int dummyFile = open("testfile.txt", O_CREAT | O_RDWR);
	if (dummyFile == -1){
		perror("open");
		exit(EXIT_SUCCESS);
	}
	if (ftruncate(dummyFile, 2048) == -1){
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}


	char *mappedLocation; 

	mappedLocation = mmap(NULL, 6144, PROT_READ | PROT_WRITE, MAP_SHARED, dummyFile, 0);
	if (mappedLocation == MAP_FAILED){
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	// accessing an address beyond the mapped space ( also not in the same page as the map)
	//mappedLocation[10000] = 'A';
	// output ./a.out' terminated by signal SIGSEGV (Address boundary error)

	// accessing an address in-range of the map but exceeding the file size (and not in the same page as the file) 
	//mappedLocation[5000] = 'B';
	// output: ./a.out' terminated by signal SIGBUS (Misaligned address error)

	

	exit(EXIT_SUCCESS);
}