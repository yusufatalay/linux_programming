#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <bits/getopt_core.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
	if ((argc > 4 || argc < 2) || strcmp(argv[1], "--help") == 0)
	{
		printf("%s [-n(number of lines) ] <file path>", argv[0]);
		exit(EXIT_FAILURE);
	}

	int n = 10; // if n is not provided, default it to 10 as the original tail command
	int c;
	int fd;
	int newlinecounter = 0;
	int totalnewline = 0;
	char *filename;
	off_t startprinting = 0;
	bool nflag = false;
	ssize_t readlen;
	char filecontent[BUFFER_SIZE];

	while ((c = getopt(argc, argv, "n::")) != -1)
	{
		switch (c)
		{
		case 'n':
			errno = 0;
			n = strtol(argv[2], NULL, 10);
			if (errno != 0)
			{
				perror("strtol");
				exit(EXIT_FAILURE);
			}
			nflag = true;
		}
	}

	if (nflag)
	{
		filename = argv[3];
	}
	else
	{
		filename = argv[1];
	}

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	// read file to buffer then read buffer char by char, for each new line increment the counter,
	// while incrementing, if totalnewline - newlinecounter = n ? set offset to newlinecounter : print whole file ( because in this case there are less new line than n )

	// get the total amounts of new lines

	while ((readlen = (read(fd, filecontent, BUFFER_SIZE))))
	{

		for (int i = 1; i < readlen; i++)
		{
			if (filecontent[i] == '\n')
			{
				totalnewline++;
			}
		}
	}

	// rewind the file
	lseek(fd, 0, SEEK_SET);

	if (totalnewline > n)
	{
		// set the offset to the desired place
		while ((readlen = (read(fd, filecontent, BUFFER_SIZE))))
		{

			for (int i = 1; i < readlen; i++)
			{

				startprinting++;
				if (filecontent[i] == '\n')
				{
					newlinecounter++;
				}
				if ((totalnewline - newlinecounter) == n - 1)
				{
					break;
				}
			}
		}

		lseek(fd, startprinting, SEEK_SET);
	}

	while ((readlen = read(fd, filecontent, BUFFER_SIZE)))
	{
		printf("%s", filecontent);
	}

	exit(EXIT_SUCCESS);
}