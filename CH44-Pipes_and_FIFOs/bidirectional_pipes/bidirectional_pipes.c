#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define PIPE_BUFFER_LEN 512

int main(void)
{
  int pipePtC[2]; // pipe to send data from parent to child
  int pipeCtP[2]; // pipe to send data from child to parent

  setbuf(stdin, 0);
  if (pipe(pipeCtP) == -1)
  {
    perror("pipe");
  }

  if (pipe(pipePtC) == -1)
  {
    perror("pipe");
  }

  switch (fork())
  {
  case -1:
    perror("fork");
  case 0:
    // Child
    char childBuffer[PIPE_BUFFER_LEN];
    char *childEndOfLine;
    // close write end of the PtC pipe and read end of the CtP pipe
    if (close(pipePtC[1]) == -1)
    {
      fprintf(stderr, "close: child");
      _exit(EXIT_FAILURE);
    }
    if (close(pipeCtP[0]) == -1)
    {
      fprintf(stderr, "close: child");
      _exit(EXIT_FAILURE);
    }

    // child loop
    for (;;)
    {
      // listen pipePtC, if there is any data,
      if (read(pipePtC[0], childBuffer, PIPE_BUFFER_LEN) == -1)
      {
        perror("read - child");
      }
      // remove the part after the new line
      childEndOfLine = strchr(childBuffer, '\n');
      if (childEndOfLine)
      {
        childEndOfLine++;
        *childEndOfLine = '\0';
      }
      // convert it to uppercase and write that data to pipeCtP
      for (int i = 0; childBuffer[i]; i++)
      {
        childBuffer[i] = toupper(childBuffer[i]);
      }
      
      if (write(pipeCtP[1], childBuffer, strlen(childBuffer)) != strlen(childBuffer))
      {
        perror("write - child");
      }
    }

  default:
    // Parent
    char parentBuffer[PIPE_BUFFER_LEN];
    char *parentEndOfLine;
    // close write end of the CtP pipe and read end of the PtC pipe
    if (close(pipePtC[0]) == -1)
    {
      fprintf(stderr, "close: parent");
      exit(EXIT_FAILURE);
    }
    if (close(pipeCtP[1]) == -1)
    {
      fprintf(stderr, "close: paren");
      exit(EXIT_FAILURE);
    }

    // parent loop
    for (;;)
    {
      // read from the stdin, and write it to pipePtC
      if (read(STDIN_FILENO, parentBuffer, PIPE_BUFFER_LEN) == -1)
      {
        perror("read - parent");
      }

      if (write(pipePtC[1], parentBuffer, PIPE_BUFFER_LEN) == -1)
      {
        perror("write - parent");
      }

      // read from pipeCtP, and write it to stdout
      if (read(pipeCtP[0], parentBuffer, PIPE_BUFFER_LEN) == -1)
      {
        perror("read - parent");
      }
      // remove the part after the new line
      parentEndOfLine = strchr(parentBuffer, '\n');
      if (parentEndOfLine)
      {
        parentEndOfLine++;
        *parentEndOfLine = '\0';
      }
      if (write(STDOUT_FILENO, parentBuffer, strlen(parentBuffer)) != strlen(parentBuffer))
      {
        perror("write - parent");
      }
    }
  }
  // Code should not reach here
  exit(EXIT_FAILURE);
}
