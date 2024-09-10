#include <stdint.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void start_clock(void);
void end_clock(long int totalSize);

static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

int main(int argc, char *argv[])
{
	if (argc != 3 || strcmp(argv[0], "--help") == 0)
	{
		printf("Usage: %s number_of_block block_size\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	long blockSize;
	int blockCount;
	char *endptr;

	errno = 0;
	blockSize = strtol(argv[1], &endptr, 10);
	if (errno != 0)
	{
		perror("strtol");
		exit(EXIT_FAILURE);
	}

	errno = 0;
	blockCount = strtol(argv[2], &endptr, 10);
	if (errno != 0)
	{
		perror("strtol");
		exit(EXIT_FAILURE);
	}

	pid_t childPID;
	int childStatus;
	int band[2];
	char writeBuf[blockSize];
	char readBuf[blockSize];

	// band[0] for reading band[1] for writing
	if (pipe(band) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	switch (childPID = fork())
	{
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);
	case 0:
		// child writes , close the reading end
		close(band[0]);
		for (int i = 0; i < blockCount; i++)
		{
			write(band[1], writeBuf, blockSize);
		}
		close(band[1]);
		_exit(EXIT_SUCCESS);
	default:
		start_clock();
		// parent reads, close the writing end
		close(band[1]);

		for (int i = 0; i < blockCount; i++)
		{
			read(band[0], readBuf, blockSize);
		}

		if (wait(&childStatus) == -1)
		{
			perror("wait");
			exit(EXIT_FAILURE);
		}

		end_clock(blockCount * blockSize);
		exit(EXIT_SUCCESS);
	}

	// code never reaches here
	return 0;
}

void start_clock()
{
	st_time = times(&st_cpu);
}

void end_clock(long int totalSize)
{
	en_time = times(&en_cpu);
	long ticks_per_second = sysconf(_SC_CLK_TCK);
	double elapsed_time = (double)(en_time - st_time) / ticks_per_second;

	printf("Total Time: %.2f seconds, Total Data Transferred: %ld bytes\nBits per Second: %.2f bps\n",
	       elapsed_time,
	       totalSize,
	       (totalSize * 8) / elapsed_time);
}