#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
        printf("%d] %s\n", i + 1, argv[i]);

    for (;;)
    {
        usleep(30000);
    }
    return 0;
}
