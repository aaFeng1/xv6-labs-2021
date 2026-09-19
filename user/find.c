#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *fliename)
{
  char buf[512];
  char *p;
  int fd;
  struct dirent de;
  struct stat st;

  // printf("-->%s\n", path);

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type)
  {
  case T_FILE:
    char *fname = strrchr(path, '/') + 1;
    // printf("%s\n", fname);
    if (strcmp(fname, fliename) == 0)
    {
      printf("%s\n", path);
    }
    break;
  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      int len = strlen(de.name);
      memmove(p, de.name, len);
      p[len] = '\0';
      // if (stat(buf, &st) < 0)
      // {
      //   printf("ls: cannot stat %s\n", buf);
      //   continue;
      // }
      // printf("%s %d %d %d\n", buf, st.type, st.ino, st.size);

      find(buf, fliename);
      memset(p, 0, DIRSIZ);
    }
    break;
  }
  close(fd);

  return;
}

int main(int argc, char *argv[])
{
  char *path;
  char *filename;
  if (argc != 3 && argc != 2)
  {
    fprintf(2, "Usage: find <path> <filename> OR find <filename>\n");
    exit(1);
  }
  if (argc == 2)
  {
    path = ".";
    filename = argv[1];
  }
  else
  {
    // argc==3
    path = argv[1];
    filename = argv[2];
  }

  // printf("find:find\n");
  // printf("%s %s\n", path, filename);

  find(path, filename);

  exit(0);
}
