#include "header.h"

/* Функция, приводящая аргумент типа char* к int */
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

char * slashes_distroyer(char * str) {
    int slash_found = FALSE;
    int i = 0, j = 0;
    char buffer_string[255];

    memset(buffer_string, 0, sizeof(buffer_string));

    while (str[i] != '\0') {
        if (str[i] != '/') {
            buffer_string[j] = str[i];
            j++;
        } else {
            if (slash_found == FALSE) {
                slash_found = TRUE;
                buffer_string[j] = str[i];
                j++;
            }
        }
        i++;
    }
    buffer_string[i] = '\0';
    memset(str, 0, sizeof(*str));
    strncpy(str, buffer_string, sizeof(buffer_string));
    return str;
}

void debug_print(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n')
            printf("/n");
        else if (str[i] == ' ')
            printf("_");
        else if (str[i] == '\0')
            printf("/0\n");
        else
            printf("%c", str[i]);
        i++;
    }
    if (str[i] == '\0') {
        printf("/0!\n");
    }
    printf("\n");
}

int state_validator(char *str) {
    int i = 0, check_counter = 0;
    char state_buffer[255] = "EMPTY";

    do {
        if ((str[i] != ' ') && (str[i] != '\0')) {
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
                            break;
                        case 1:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 23))
                                return INCORRECT_EXPRESSION;
                            break;
                        case 2:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 30))
                                return INCORRECT_EXPRESSION;
                            break;
                        case 3:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 11))
                                return INCORRECT_EXPRESSION;
                            break;
                        default:
                            if ((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 99))
                                return INCORRECT_EXPRESSION;
                            break;
                    }
                } else {
                    switch (check_counter) {
                        case 0:
                             if (((strtoint(state_buffer) < 0) || (strtoint(state_buffer) > 59)) && (strcmp(state_buffer, "*") != 0)) {
                                return INCORRECT_EXPRESSION;
                             }
                            break;
                        case 1:
                            if (((strtoint(state_buffer) < 0) || (strtoint(state_buffer) > 23)) && (strcmp(state_buffer, "*") != 0)) {
                                return INCORRECT_EXPRESSION;
                            }
                            break;
                        case 2:
                            if (((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 31)) && (strcmp(state_buffer, "*") != 0)) {
                                return INCORRECT_EXPRESSION;
                             }
                            break;
                        case 3:
                            if (((strtoint(state_buffer) < 1) || (strtoint(state_buffer) > 12)) && (strcmp(state_buffer, "*") != 0)) {
                                return INCORRECT_EXPRESSION;
                             }
                            break;
                        default:
                            if (((strtoint(state_buffer) < 16) || (strtoint(state_buffer) > 116)) && (strcmp(state_buffer, "*") != 0))
                                return INCORRECT_EXPRESSION;
                            break;
                    }
                } 
                if ((strcmp(state_buffer, "EMPTY") != 0) && (str[i] != '\0')) {
                    memset(state_buffer, 0, sizeof(state_buffer));
                    strcpy(state_buffer, "EMPTY");
                }
                check_counter++;
            }
            else{
                return INCORRECT_EXPRESSION;
            }
        }
        i++;
    }  while (check_counter <= 4);

    if (strcmp(state_buffer, "EMPTY") == 0){
        return INCORRECT_EXPRESSION;
    }
    else {
        return CORRECT_EXPRESSION;
    }
} 

int validator(char *str) {
    int i = 0, spaces = 0, slashes = 0, stars = 0;

    while (str[i] != '\0') {
        if ((!isdigit(str[i])) && (str[i] != ' ') && (str[i] != '\0') && (str[i] != '*') && (str[i] != '\n') && (str[i] != '/')) {
            return INCORRECT_EXPRESSION;
        }

        if (str[i] == ' ')  
            spaces++;
        if (str[i] == '*'){
            stars++;
            if ((i > 0) && ((str[i - 1] == '*') || (str[i - 1] == '/'))){
                return INCORRECT_EXPRESSION;
            }
            if (!((i < 253) && ((str[i + 1] == '\n') || (str[i + 1] == ' ') || (str[i + 1] == '\0')))) {
                return INCORRECT_EXPRESSION;
            }
        }
        if (str[i] == '/') {
            slashes++;
            if ((i < 253) && (!isdigit(str[i + 1]))) {
                return INCORRECT_EXPRESSION;    
            } 
            if ((i > 0) && (str[i - 1] != ' ')){
                return INCORRECT_EXPRESSION;
            } 
        }
        i++;
        if (str[i] == '\n')
            str[i] = '\0';
    }

    if (slashes > 5)
        return INCORRECT_EXPRESSION;
    else if (slashes > 1) {
        str = slashes_distroyer(str);
        printf("String was changed to %s\n", str);    
    }

    if ((spaces == 4) && (state_validator(str) == CORRECT_EXPRESSION) && (stars <= 4)) {
        return CORRECT_EXPRESSION;
    }
    else
        return INCORRECT_EXPRESSION;
}