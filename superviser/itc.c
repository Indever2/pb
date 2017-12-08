#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#include "itc.h"

int send_msg(const char *fifo_file, msg_type_t type, const char *payload)
{
    msg_t msg;
    msg.type = type;
    strncpy(msg.payload, payload, 16);

    char buf[sizeof(msg)] = { 0 };
    memcpy(buf, &msg, sizeof(msg));

    if (access(fifo_file, F_OK))
      return -1;

    int fifo_fd = open(fifo_file, O_WRONLY | O_NONBLOCK);
    write(fifo_fd, buf, sizeof(msg));
    close(fifo_fd);
}

int main(int argc, char **argv)
{
    if (argc < 3) return -1;

    char fifo_file[64] = { 0 };

    snprintf(fifo_file, 63, "%s%s", FIFO_PREFIX, argv[1]);
    printf("fifo: %s\n", fifo_file);

    if (!strncmp(argv[2], "pid", 3))
    {
        if (argc < 4) return -1;
        printf("pid %s\n", argv[3]);
        if (send_msg(fifo_file, msg_PID, argv[3]) == -1)
        {
            perror("access:");
            exit(-1);
        }
    }
    else if (!strncmp(argv[2], "stop",4))
    {
        printf("stop\n");
        if (send_msg(fifo_file, msg_STOP, "stop") == -1)
        {
            perror("access:");
            exit(-1);
        }
    }
    else if (!strncmp(argv[2], "check", 5))
    {
        printf("check\n");
        if (send_msg(fifo_file, msg_CHECK, "check") == -1)
        {
            perror("access:");
            exit(-1);
        }
    }
    else if (!strncmp(argv[2], "restart", 7))
    {
        printf("restart\n");
        if (send_msg(fifo_file, msg_RESTART, "restart") == -1)
        {
            perror("access:");
            exit(-1);
        }
    }
    else
    {
        printf("nothing\n");
        return - 1;
    }
    return 0;
}
