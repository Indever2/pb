#ifndef ITC_H
#define ITC_H

const char *FIFO_PREFIX = "/home/indever/tmp/superviser.";

enum msg_type_e
{
    msg_PID,
    msg_STOP,
    msg_RESTART,
    msg_CHECK,
} typedef msg_type_t;

struct msg_s
{
    msg_type_t type;
    char payload[16];
} typedef msg_t;

#endif /* ITC_H */
