/* * * * * * * * * * * * * * * * * * * * * * *
*                                             *
*   Задание 7.                                *
*   Пример работы  функций fork() и exec().   *
*                                             *                
* * * * * * * * * * * * * * * * * * * * * * */

#include "unp.h"

/*функция, превращающая введенную в виде аргумента команду в команду $PATH*/
char *make_path(char *arg){ 

    char *str = (char *)malloc(sizeof(char *) * 64); 
    strcpy(str, "/bin/");
    strcat(str, arg);

    printf("%s", str);

    return str;

}

int main(int argc, char **argv) {

    pid_t pid = fork();

    if (!pid) {

        char **argv2 = argv + 1;
        execv(make_path(argv[1]), argv2); //вызов exec

    } else {

        waitpid(pid, NULL, NULL); //ждем завершения потомка.
        return 0;

    }

}