#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        fprintf(2, "Usage: pingpong\n");
        exit(1);
    }

    int pip0[2], pip1[2];

    if (pipe(pip0) < 0 || pipe(pip1) < 0)
    {
        fprintf(2, "Pipe fail\n");
        exit(1);
    }

    int son = fork();

    if (son == 0)
    {
        // son
        close(pip0[1]);
        close(pip1[0]);
        char c;
        if (read(pip0[0], &c, 1) < 0)
        {
            fprintf(2, "son: read failed\n");
            exit(1);
        }
        close(pip0[0]);
        printf("%d: received ping\n", getpid());
        if (write(pip1[1], &c, 1) < 0)
        {
            fprintf(2, "son: write failed\n");
            exit(1);
        }
        close(pip1[1]);
    }
    else
    {
        // father
        close(pip0[0]);
        close(pip1[1]);
        char c = 'a';
        if (write(pip0[1], &c, 1) < 0)
        {
            fprintf(2, "father: write failed\n");
            exit(1);
        }
        close(pip0[1]);
        if (read(pip1[0], &c, 1) < 0)
        {
            fprintf(2, "son: read failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());
        close(pip1[0]);
    }

    exit(0);
}
