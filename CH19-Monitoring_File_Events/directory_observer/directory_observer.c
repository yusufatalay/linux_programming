#define _XOPEN_SOURCE 500

#include <limits.h>
#include <stdio.h>
#include <ftw.h>
#include <sys/inotify.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int inotifyFD;

static int
directory_action(const char *fpath, const struct stat *sb,
		 int tflag, struct FTW *ftwbuf)
{
	if (tflag == FTW_D)
	{
		if ((inotify_add_watch(inotifyFD, fpath, IN_ALL_EVENTS)) == -1)
		{
			perror("inotify_add_watch");
			exit(EXIT_FAILURE);
		}
	}

	return 0; /* To tell nftw() to continue */
}

static void
displayInotifyEvent(struct inotify_event *i)
{
	printf("\twd =%2d; ", i->wd);
	if (i->cookie > 0)
	{
		printf("cookie = %4d;", i->cookie);
	}

	printf("mask = ");
	if (i->mask & IN_ACCESS)
		printf("IN_ACCESS");
	if (i->mask & IN_ATTRIB)
		printf("IN_ATTRIB");
	if (i->mask & IN_CLOSE_NOWRITE)
		printf("IN_CLOSE_NOWRITE");
	if (i->mask & IN_CLOSE_WRITE)
		printf("IN_CLOSE_WRITE");
	if (i->mask & IN_CREATE)
		printf("IN_CREATE");
	if (i->mask & IN_DELETE)
		printf("IN_DELETE");
	if (i->mask & IN_DELETE_SELF)
		printf("IN_DELETE_SELF");
	if (i->mask & IN_IGNORED)
		printf("IN_IGNORED");
	if (i->mask & IN_ISDIR)
		printf("IN_ISDIR");
	if (i->mask & IN_MODIFY)
		printf("IN_MODIFY");
	if (i->mask & IN_MOVE_SELF)
		printf("IN_MOVE_SELF");
	if (i->mask & IN_MOVED_FROM)
		printf("IN_MOVED_FROM");
	if (i->mask & IN_MOVED_TO)
		printf("IN_MOVED_TO");
	if (i->mask & IN_OPEN)
		printf("IN_OPEN");
	if (i->mask & IN_Q_OVERFLOW)
		printf("IN_Q_OVERFLOW");
	if (i->mask & IN_UNMOUNT)
		printf("IN_UNMOUNT");
	printf("\n");

	if (i->len > 0)
	{
		printf("\t\t name = %s\n", i->name);
	}
}

int main(int argc, char *argv[])
{

	if (argc != 2 || strcmp(argv[1], "--help") == 0)
	{
		printf("Usage: %s <path_to_directory>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char buf[BUF_LEN] __attribute__((aligned(8)));
	ssize_t numRead;
	char *p;
	struct inotify_event *event;

	inotifyFD = inotify_init();
	if (inotifyFD == -1)
	{
		perror("inotify_init");
		exit(EXIT_FAILURE);
	}

	if (nftw(argv[1], directory_action, 1, 0) == -1)
	{
		perror("nftw");
		exit(EXIT_FAILURE);
	}

	for (;;)
	{ // read events forever
		numRead = read(inotifyFD, buf, BUF_LEN);
		if (numRead == 0)
		{
			perror("read() from inotify fd returned 0");
			exit(EXIT_FAILURE);
		}

		if (numRead == -1)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}

		printf("Read %ld bytes from inotify fd\n", (long)numRead);

		// Process all  of the events in buffer returned by read()

		for (p = buf; p < buf + numRead;)
		{
			event = (struct inotify_event *)p;
			displayInotifyEvent(event);

			// If a directory is created, add a watch to it

			if (event->mask & IN_CREATE && event->mask & IN_ISDIR)
			{

				if (nftw(argv[1], directory_action, 1, 0) == -1)
				{
					perror("nftw");
					exit(EXIT_FAILURE);
				}
			}

			p += sizeof(struct inotify_event) + event->len;
		}
	}
}