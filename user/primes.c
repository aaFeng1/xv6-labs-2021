#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute__((noreturn)) void filterPrime(int readFd)
{
  int fd[2];
  int son;

  int prime;
  if (read(readFd, &prime, sizeof(int)) <= 0)
  {
    fprintf(2, "%d: read failed\n", getpid());
    exit(1);
  }
  printf("prime %d\n", prime);
  sleep(1);
  int d;
  int hasson = 0;
  while (read(readFd, &d, sizeof(int)) > 0)
  {
    // printf("%d\n", d);
    if (d % prime == 0)
    {
      continue;
    }
    if (hasson == 0)
    {
      if (pipe(fd) < 0)
      {
        fprintf(2, "%d: pipe failed\n", getpid());
        exit(1);
      }
      son = fork();
      if (son == -1)
      {
        fprintf(2, "%d: fork failed\n", getpid());
        exit(1);
      }
      if (son == 0)
      {
        // son
        close(fd[1]);
        filterPrime(fd[0]);
      }
      else
      {
        // father
        close(fd[0]);
      }
      hasson = 1;
    }

    if (write(fd[1], &d, sizeof(int)) < 0)
    {
      fprintf(2, "%d: write %d failed\n", getpid(), d);
      exit(1);
    }
  }
  close(readFd);
  if (hasson == 1)
  {
    close(fd[1]);
    wait(0);
  }
  // printf("pid %d exit\n", getpid());
  exit(0);
}

int main(int argc, char *argv[])
{
  int fd[2];
  if (pipe(fd) < 0)
  {
    fprintf(2, "%d: pipe failed\n", getpid());
    exit(1);
  }
  int son = fork();
  if (son == -1)
  {
    fprintf(2, "%d: fork failed\n", getpid());
    exit(1);
  }

  if (son == 0)
  {
    // son
    close(fd[1]);
    filterPrime(fd[0]);
  }
  else
  {
    // father
    close(fd[0]);
    for (int i = 2; i <= 35; ++i)
    {
      if (write(fd[1], &i, sizeof(int)) < 0)
      {
        fprintf(2, "%d: wirte %d failed\n", getpid(), i);
        exit(1);
      }
    }
    close(fd[1]);
    wait(0);
  }
  exit(0);
}
