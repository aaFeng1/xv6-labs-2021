#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
  char buf[512], *p;
  int len, remain = 512;

  p = buf;

  while ((len = read(0, p, remain)) > 0)
  {
    p += len;
    remain -= len;
  }

  // printf("%s\n",buf);

  int numarg = argc - 1;
  char *arg[MAXARG] = {0};
  for (int i = 1; i < argc; ++i)
  {
    arg[i - 1] = argv[i];
  }

  int inparam = 0;
  p = buf;
  len = strlen(buf);

  for (int i = 0; i < len; ++i)
  {
    if (buf[i] == '\n' || buf[i] == ' ')
    {
      // printf("-->%d\n", i);
      if (!inparam)
        continue;
      if (numarg == MAXARG)
      {
        fprintf(2, "xargs: too many args\n");
        exit(1);
      }
      arg[numarg++] = p;
      buf[i] = '\0';
      p = &buf[i + 1];
      inparam = 0;
    }
    else
    {
      inparam = 1;
    }
  }
  if (inparam == 1)
  {
    if (numarg == MAXARG)
    {
      fprintf(2, "xargs: too many args\n");
      exit(1);
    }
    arg[numarg++] = p;
  }

  // for (int i = 0; i < numarg; ++i)
  // {
  //   printf("%d : %s\n", i, arg[i]);
  // }

  int sonp = fork();
  if (sonp == -1)
  {
    fprintf(2, "xargs: fork fail\n");
    exit(1);
  }

  if (sonp == 0)
  {
    // son
    exec(arg[0], arg);
  }
  else
  {
    // father
    wait(0);
  }

  exit(0);
}
