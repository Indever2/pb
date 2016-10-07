#ifndef __RESOURCES_H
#define __RESOURCES_H

/* Заголовочные файлы */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>  
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

int strtoint(const char *);

/* Константы */
#define MODEM_NAME_1 "/dev/ttyUSB30"  /* ttyUSB30 - имя файла модема в директории /dev/ на устройстве */
#define MODEM_NAME_2 "/dev/ttyUSB10"  /* ttyUSB10 - имя файла модема в директории /dev/ на устройстве */
#define PATH_TO_FIFO "/tmp/atfile"

#endif /* resourses.h */