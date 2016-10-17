#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define INCORRECT_EXPRESSION   -1
#define CORRECT_EXPRESSION      1      

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

/*
int days_in_month(int month) {
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 2:
            return 28;
        default:
            return 30;
    }
}*/

void need_update(char **upd_conf, struct tm * cur_time, time_t last_update) {
    printf("%.2f\n", difftime(mktime(cur_time), last_update));

    if (strcmp(upd_conf[0], "*") == 0) {
        printf("min:\tyes\n");
    } else if (upd_conf[0][0] != '/') {
        if (strtoint(upd_conf[0]) == cur_time->tm_min)
            printf("min:\tyes\n");
        else
            printf("min:\tno\n");
    } else {
        if ((strtoint(upd_conf[0] + 1) * 60) <= difftime(mktime(cur_time), last_update))
            printf("min:\tREG yes\n");
        else
            printf("min:\tREG no\n");
    }

    if (strcmp(upd_conf[1], "*") == 0) {
        printf("hour:\tyes\n");
    } else if (upd_conf[1][0] != '/') {
        if (strtoint(upd_conf[1]) == cur_time->tm_hour)
            printf("hour:\tyes\n");
        else
            printf("hour\tno\n");
    } else {
        if ((strtoint(upd_conf[1] + 1) * 60 * 60) <= difftime(mktime(cur_time), last_update))
            printf("hour:\tREG yes\n");
        else
            printf("hour:\tREG no\n");
    }

    if (strcmp(upd_conf[2], "*") == 0) {
        printf("day:\tyes\n");
    } else if (upd_conf[2][0] != '/') {
        if (strtoint(upd_conf[2]) == cur_time->tm_mday)
            printf("day:\tyes\n");
        else
            printf("day:\tno\n");
    } else {
        if ((strtoint(upd_conf[2] + 1) * 60 * 60 * 24) <= difftime(mktime(cur_time), last_update))
            printf("day:\tREG yes\n");
        else
            printf("day:\tREG no\n");
    }

    if (strcmp(upd_conf[3], "*") == 0) {
        printf("month:\tyes\n");
    } else if (upd_conf[3][0] != '/') {
        if (strtoint(upd_conf[3]) == (cur_time->tm_mon + 1))
            printf("month:\tyes\n");
        else
            printf("month:\tno\n");
    } else {
        if (strtoint(upd_conf[3] + 1) * 60 * 60 * 24 * 30 <= difftime(mktime(cur_time), last_update))
            printf("month:\tREG yes\n");
        else
            printf("month:\tREG no\n");
    }

    if (strcmp(upd_conf[4], "*") == 0) {
        printf("year:\tyes\n");          
    } else if (upd_conf[4][0] != '/') {
        if (strtoint(upd_conf[4]) == (cur_time->tm_year - 100))
            printf("year:\tyes\n");
        else
            printf("year:\tno\n");
    } else {
        if ((strtoint(upd_conf[4] + 1) * 60 * 60 * 24 * 365) <= difftime(mktime(cur_time), last_update))
            printf("year:\tREG yes\n");
        else
            printf("year:\tREG no\n");
    }
}

int main(int argc, char **argv) {
    /* No validation in this program */
    char **upd_conf = NULL;
    char buf[255], str[255];

    int i = 0, j = 0, value_counter = 0;

    size_t needed_mem = 0x0000;

    long int cur_time = time(NULL), last_update = time(NULL);
    last_update -= 60 * 30;

    struct tm * structured_time = localtime(&last_update);
    struct tm * backup_st = malloc(sizeof(struct tm));
    memmove(backup_st, structured_time, sizeof(struct tm));
    structured_time = localtime(&cur_time);
    
    fgets(str, 255, stdin);
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
    printf("current:\t%d %d %d %d %d\n", structured_time->tm_min, structured_time->tm_hour, 
           structured_time->tm_mday, structured_time->tm_mon + 1, structured_time->tm_year - 100);
    printf("last_upd:\t%d %d %d %d %d\n", backup_st->tm_min, backup_st->tm_hour, 
           backup_st->tm_mday, backup_st->tm_mon + 1, backup_st->tm_year - 100);
    need_update(upd_conf, structured_time, last_update);
    
    for (i = 0; i < 5; i++) {
        free(upd_conf[i]);
    }
    free(upd_conf);
    free(backup_st);
    return 0;
}