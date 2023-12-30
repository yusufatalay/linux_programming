#define _DEFAULT_SOURCE
#include <dirent.h>
#include <err.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROC_PATH "/proc/"
#define FD_PATH "/fd/"
#define EXE_PATH "/exe"
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	// if (argc != 2 || strcmp(argv[1], "--help") == 0)
	//{
	//	printf("Usage: %s <file_path>", argv[0]);
	//	exit(EXIT_FAILURE);
	// }

	DIR *procDIR;
	struct dirent *procDirContent;

	char *processFDDIRPath;
	char *processFDFilePath;
	char *processEXEFilePath;
	char exeFileContent[BUF_SIZE];
	char linkFileContent[BUF_SIZE];
	ssize_t exeFileLen;
	ssize_t linkFileLen;

	DIR *procFDDIR;
	struct dirent *procFDDirContent;

	char *endptr = "";

	procDIR = opendir(PROC_PATH);
	if (procDIR == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	while ((procDirContent = readdir(procDIR)) != NULL)
	{
		if ((strcmp(procDirContent->d_name, ".") == 0) ||
		    (strcmp(procDirContent->d_name, "..") == 0))
		{
			continue;
		}
		if (procDirContent->d_type == DT_DIR)
		{
			// make sure the filename is numeric (all processes have numeric names)
			strtol(procDirContent->d_name, &endptr, 10);
			if (*endptr == '\0')
			{
				// We've found process directory
				// construct path to that directory  /proc/<name>/status
				processFDDIRPath =
				    malloc(strlen(PROC_PATH) + strlen(procDirContent->d_name) +
					   strlen(FD_PATH) + 1);
				if (processFDDIRPath == NULL)
				{
					perror("malloc");
					exit(EXIT_FAILURE);
				}
				processFDDIRPath[strlen(PROC_PATH) +
						 strlen(procDirContent->d_name)] = '\0';
				processFDDIRPath = strcpy(processFDDIRPath, PROC_PATH);
				processFDDIRPath =
				    strcat(processFDDIRPath, procDirContent->d_name);
				processFDDIRPath = strcat(processFDDIRPath, FD_PATH);

				processEXEFilePath = malloc(strlen(PROC_PATH) + strlen(procDirContent->d_name) +
							    strlen(EXE_PATH) + 1);
				if (processEXEFilePath == NULL)
				{
					perror("malloc");
					exit(EXIT_FAILURE);
				}

				processEXEFilePath[strlen(PROC_PATH) + strlen(procDirContent->d_name) +
						   strlen(EXE_PATH) + 1] = '\0';

				processEXEFilePath = strcpy(processEXEFilePath, PROC_PATH);
				processEXEFilePath =
				    strcat(processEXEFilePath, procDirContent->d_name);
				processEXEFilePath = strcat(processEXEFilePath, EXE_PATH);

				exeFileLen = readlink(processEXEFilePath, exeFileContent, BUF_SIZE);
				if (exeFileLen == -1)
				{
					free(processEXEFilePath);
					continue;
				}
				exeFileContent[exeFileLen] = '\0';

				// traverse the /proc/PID/fd path, and use readlink
				procFDDIR = opendir(processFDDIRPath);
				if (procFDDIR == NULL)
				{
					perror("opendir");
					exit(EXIT_FAILURE);
				}

				while ((procFDDirContent = readdir(procFDDIR)) != NULL)
				{
					if ((strcmp(procFDDirContent->d_name, ".") == 0) ||
					    (strcmp(procFDDirContent->d_name, "..") == 0))
					{
						continue;
					}
					// read the content of the /proc/PID/fd  with readlink function
					// construct file path for /proc/PID/fd

					processFDFilePath =
					    malloc(strlen(processFDDIRPath) + strlen(procFDDirContent->d_name) + 1);
					if (processFDFilePath == NULL)
					{
						perror("malloc");
						exit(EXIT_FAILURE);
					}
					processFDFilePath[strlen(processFDDIRPath) + strlen(procFDDirContent->d_name)] = '\0';

					processFDFilePath = strcpy(processFDFilePath, processFDDIRPath);

					processFDFilePath =
					    strcat(processFDFilePath, procFDDirContent->d_name);

					linkFileLen = readlink(processFDFilePath, linkFileContent, sizeof(linkFileContent) - 1);
					if (linkFileLen == -1)
					{
						free(processFDFilePath);
						continue;
					}
					linkFileContent[linkFileLen] = '\0';

					// compare the given file path (FULL path) with the linkFileContent,
					// if they match print it out
					if (strcmp(argv[1], linkFileContent) == 0){
						printf("%s\n", exeFileContent);
						break;
					}

					free(processFDFilePath);
				}
				
				free(processEXEFilePath);
				free(processFDDIRPath);
			}
		}
	}

	closedir(procDIR);

	return 0;
}