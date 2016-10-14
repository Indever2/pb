#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/* Функция, приводящая аргумент типа char* к виду int */
int strtoint(char * in) { 
    int i = 0, j = 0;
    int len = 0, sum = 0, multipler = 1; // multipler - множитель, нужен для правилльной позиционировки по разрядам
    len = strlen(in);
    
    for (i = 0; i < len; i++) {
        for (j = 0; j < i; j++)
            multipler *= 10; // После каждой итерации цикла множитель увеличивается в 10 раз.
        sum += (in[len - i - 1] - '0') * multipler; // Прибавляаем к сумме число, полученное умножением извлеченного символа на множитель.
        multipler = 1; // Обнуляем множитель
    }
    
    return sum;
}

void need_update(char **upd_conf, struct tm * cur_time/*, struct tm * last_update*/) {
    if ((strcmp(upd_conf[0], "*") != 0) && (upd_conf[0][0] != '/')) {
        if (strtoint(upd_conf[0]) == cur_time->tm_min)
            printf("min:\tyes\n");
        else
            printf("min:\tno\n");
    }
    if ((strcmp(upd_conf[1], "*") != 0) && (upd_conf[1][0] != '/')) {
        if (strtoint(upd_conf[1]) == cur_time->tm_hour)
            printf("hour:\tyes\n");
        else
            printf("hour\tno\n");
    }
    if ((strcmp(upd_conf[2], "*") != 0) && (upd_conf[2][0] != '/')) {
        if (strtoint(upd_conf[2]) == cur_time->tm_mday)
            printf("day:\tyes\n");
        else
            printf("day:\tno\n");
    }
    if ((strcmp(upd_conf[3], "*") != 0) && (upd_conf[3][0] != '/')) {
        if (strtoint(upd_conf[3]) == (cur_time->tm_mon + 1))
            printf("month:\tyes\n");
        else
            printf("month:\tno\n");
    }
    if ((strcmp(upd_conf[4], "*") != 0) && (upd_conf[4][0] != '/')) {
        if (strtoint(upd_conf[4]) == (cur_time->tm_year - 100))
            printf("year:\tyes\n");
        else
            printf("year:\tno\n");
    }
}

int main(int argc, char **argv) {
    /* No validation in this program */
    char **upd_conf = NULL;
    char buf[255], str[255];
    fgets(str, 255, stdin);
    int i = 0, j = 0, value_counter = 0;
    size_t needed_mem = 0x0000;
    time_t cur_time = time(NULL);
    struct tm * structured_time = localtime(&cur_time);
    
    upd_conf = malloc(sizeof(char *) * 5);
    
    while (str[i] != '\0') {
        if ((str[i] != ' ') && (str[i] != '\n')) {
            buf[j] = str[i];
            j++;
        } else {
            buf[j] = '\0';
            needed_mem = snprintf(NULL, 0, "%s", buf);
            upd_conf[value_counter] = malloc(sizeof(char) * (needed_mem += 2));
            snprintf(upd_conf[value_counter], needed_mem, "%s", buf);
            value_counter++;
            j = 0;
            memset(&buf, 0, sizeof(buf));
        }
        i++;
    }
    
    printf("%s %s %s %s %s\n", upd_conf[0], upd_conf[1], upd_conf[2], upd_conf[3], upd_conf[4]);
    printf("%d %d %d %d %d\n", structured_time->tm_min, structured_time->tm_hour, 
           structured_time->tm_mday, structured_time->tm_mon + 1, structured_time->tm_year - 100);
    need_update(upd_conf, structured_time);
    
    for (i = 0; i < 5; i++) {
        free(upd_conf[i]);
    }
    free(upd_conf);
    return 0;
}