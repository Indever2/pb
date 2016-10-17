#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

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



int state_validator(char *str) {
    int i = 0, check_counter = 0;
    char state_buffer[255] = "EMPTY";

    while (str[i] != '\0') {
        if (str[i] != ' ') {
            if (strcmp(state_buffer, "EMPTY") == 0)
                memset(state_buffer, 0, sizeof(state_buffer));
            strncat(state_buffer, &str[i], 1);
        } else {
            if (strcmp(state_buffer, "EMPTY") != 0) {
                if (state_buffer[0] == '/') {
                    state_buffer[0]++;
                    switch (check_counter) {
                        case 0:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 59))
                                return INCORRECT_EXPRESSION;
                            check_counter++;
                            break;
                        case 1:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 59))
                                return INCORRECT_EXPRESSION;
                            check_counter++;
                            break;
                        case 2:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 23))
                                return INCORRECT_EXPRESSION;
                            check_counter++;
                            break;
                        case 3:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 11))
                                return INCORRECT_EXPRESSION;
                            check_counter++;
                            break;
                        default:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 99))
                                return INCORRECT_EXPRESSION;
                            check_counter++;
                            break;
                    }
                }
                memset(state_buffer, 0, sizeof(state_buffer));
                strcpy(state_buffer, "EMPTY");
            }
            else
                return INCORRECT_EXPRESSION;
        }
        i++;
    }
    if (strcmp(state_buffer, "EMPTY") == 0)
        return INCORRECT_EXPRESSION;
    else {
        return CORRECT_EXPRESSION;
    }
} 

void debug_print(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n')
            printf("__END\n");
        else if (str[i] == ' ')
            printf("_");
        else
            printf("%c", str[i]);
        i++;
    }
    printf("\n");
}

int validator(char *str) {
    int i = 0, spaces = 0, slashes = 0, stars = 0;

    while (str[i] != '\0') {

        if ((!isdigit(str[i])) && (str[i] != ' ') && (str[i] != '\0') && (str[i] != '*') && (str[i] != '\n') && (str[i] != '/')) {
            printf("i:\t%d\n", i);
            return INCORRECT_EXPRESSION;
        }

        if (str[i] == ' ')  
            spaces++;
        if (str[i] == '*'){
            stars++;
            if ((i > 0) && ((str[i - 1] == '*') || (str[i - 1] == '/'))){
                printf("1stars i:\t%d\n", i);
                return INCORRECT_EXPRESSION;
            }
            if (!((i < 253) && ((str[i + 1] == '\n') || (str[i + 1] == ' ') || (str[i + 1] == '\0')))) {
                printf("2stars i:\t%d\n", i);
                return INCORRECT_EXPRESSION;
            }
        }
        if (str[i] == '/') {
            slashes++;
            if ((i < 253) && (!isdigit(str[i + 1]))) {
                printf("slashes i:\t%d\n", i);
                return INCORRECT_EXPRESSION;    
            } 
            if ((i > 0) && (str[i - 1] != ' ')){
                printf("slashes i:\t%d\n", i);
                return INCORRECT_EXPRESSION;
            } 
        }
        i++;
    }    

    if ((spaces == 4) && (state_validator(str) == CORRECT_EXPRESSION) && (stars <= 4) && (slashes <= 5)) {
        return CORRECT_EXPRESSION;
    }
    else
        return INCORRECT_EXPRESSION;
}

int main() {
    char buf[255];
    fgets(buf, 254, stdin);

    if (validator(buf) == CORRECT_EXPRESSION)
        printf("CORRECT\n");
    else
        printf("INCORRECT\n");
    
    return 0;
}