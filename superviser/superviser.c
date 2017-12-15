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

enum ERRORS
{
    INVALID_ARGUMENT = -1,
};

struct globals_s
{
    int fifo_fd;
    pid_t pid;
    char fifo_file[64];
    char child_name[64];
} typedef globals_t;

/* Global variables*/
globals_t Globals;
/* end */

int check_fifo(msg_t *msg)
{
    int len;

    len = read(Globals.fifo_fd, msg, sizeof(*msg));
    if (!len) return 0;

    return 1;
}

char **dup_argv(int argc, char **argv)
{
    char **argv_ = malloc(sizeof(char *) * (argc + 1));
    for (int i = 0; i < argc; i++)
    {
        argv_[i] = malloc(sizeof(argv[i]));
        strcpy(argv_[i], argv[i]);
    }
    argv_[argc] = NULL;
    return argv_;
}

void free_argv(char **argv)
{
    int i = 0;
    while (argv[i]) { free(argv[i]); i++; }
    free(argv);
}

int exec_cmd(int argc, char **argv)
{
    char **argv_ = dup_argv(argc, argv);
    int j = 0;
    while (argv_[j])
    {
        printf("DUP: %d]%s\n", j + 1, argv_[j]);
        j++;
    }

    /* Тут процесс делится надвое - родительский и дочерний процессы */
    pid_t pid = fork();

    if (!pid) /* Мы - ребенок */
    {
        Globals.pid = pid;
        execvp(argv[0], argv_);
        return 0; /* Компилятор доволен */
    }
    else /* Мы - родитель */
    {
        free_argv(argv_);
        return 0;
    }
}

int process_msg(msg_t msg)
{
    switch(msg.type)
    {
        case msg_PID:
            printf("MSG: PID = %s\n", msg.payload);
            pid_t buf_pid = atoi(msg.payload);
            if (buf_pid)
                Globals.pid = buf_pid;
                return 0;
            break;
        case msg_STOP:
            printf("MSG: STOP\n");
            return 0xa;
            break;
        case msg_RESTART:
            printf("MSG: RESTART\n");
            return 0xb;
            break;
        case msg_CHECK:
            printf("MSG: CHECK\n");
            return 0xc;
            break;
        default:
            break;
    }
    return 0;
}

int process_start(int argc, char **argv)
{
    if (!argv || !argv || !argv[0])
        return INVALID_ARGUMENT;

    int res = 0;
    char cmd[255] = { 0 };
    char args[128] = { 0 };
    char _args[128] = { 0 };
    char filename[128] = { 0 };
    FILE *f = NULL;

    msg_t msg_buf;
    memset(&msg_buf, 0, sizeof(msg_buf));

    for (int i = 0; i < argc; i++)
        printf("%d] - %s\n", i + 1, argv[i]);

    exec_cmd(argc, argv);

    daemon(0, 0);
for_loop:
    for ( ; ; )
    {
        if (check_fifo(&msg_buf))
        {
            res = process_msg(msg_buf);
        }
        if (res == 0xa) goto stop;
        if (res == 0xb) goto stop;
        if (res == 0xc) goto check;
        usleep(30000);
    }
stop:
    close(Globals.fifo_fd);
    unlink(Globals.fifo_file);
    kill(Globals.pid, 9);
    return 0;
check:
    snprintf(_args, 127, "%s", argv[0]);
    for (int i = 1; i < argc; i++)
    {
        snprintf(args, 127, "%s %s", _args, argv[i]);
        snprintf(_args, 127, "%s", args);
    }
    snprintf(filename, 127, "%s%s", CHECK_PREFIX, Globals.child_name);
    snprintf(cmd, 254, "scan.bash \"%s\"", args);
    system(cmd);
    goto for_loop;
restart:
    goto for_loop;
}

int main(int argc, char **argv)
{
    if (argc < 3) goto usage;

    sprintf(Globals.fifo_file, "%s%s", FIFO_PREFIX, argv[2]);
    sprintf(Globals.child_name, "%s", argv[2]);
    printf("fifo_file: %s\n", Globals.fifo_file);

    if (!strncmp(argv[1], "start", 5))
    {
        if (!access(Globals.fifo_file, F_OK))
        {
          printf("superviser of %s is already running! Execute superviser stop %s\n", argv[2], argv[2]);
          return 0;
        }
        if (mkfifo(Globals.fifo_file, 0777))
        {
          printf("can't create fifo channel\n");
          return -1;
        }
        Globals.fifo_fd = open(Globals.fifo_file, O_RDONLY | O_NONBLOCK);
        printf("start\n");
        process_start(argc - 2, argv + 2);
        goto halt;
    }
    else if (!strncmp(argv[1], "stop", 4))
    {
        printf("stop\n");
        char cmd[64] = { 0 };
        snprintf(cmd, 63, "./itc %s stop", argv[2]);
        system(cmd);
        goto halt;
    }
    else
    {
        goto usage;
    }

halt:
    return 0;
usage:
    printf("usage: superviser [start|stop] <command> <args (space separated)>\n");
    printf("       (c) Elizaweta Zanozina, 2017\n");
    return -1;
}
