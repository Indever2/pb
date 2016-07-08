/* * * * * * * * * * * * * * * * * * * * * * *
*     Сервер.                                 *
*     Принимает сообщения от клиентов         *
*     и добавляет их в очередь.               *
*     Из очереди читают 2 потока.             *
*     Находится в доработке.                  *
* * * * * * * * * * * * * * * * * * * * * * */

#include "unp.h"
#include <time.h>

int main() {

    /*очередь сообщений релизована через разделяемую память, т.к. обрабатывать ее будут дочерние процессы.*/
    /*Константы BUF_SIZE() и QU_SIZE() искать в unp.h*/
    char **qu = mmap(NULL, sizeof(char **) * QU_SIZE(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
    for (int i = 0; i < QU_SIZE(); i++) {

        qu[i] = mmap(NULL, sizeof(char *) * BUF_SIZE(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        snprintf(qu[i], sizeof(qu[i]), "\0");

    }

    /*Завершено ли формирование очереди*/
    volatile int *done = mmap(NULL, sizeof(int *), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
    *done = FALSE;

    int serv_sock = 0, client_sock = 0; //сокеты
    int enable = 1; //Для установки параметров сокета
    int answ_len, count = 0, is_check = 0; 
    int qu_pos = 0; //позиция в очереди

    pid_t pid = 1;

    char buffer[BUF_SIZE()]; 

    /*необходимо для проверки поступления данных на сокет*/
    fd_set fd; 
    struct timeval tv; //для тайм-аута ожидания

    struct sockaddr_in sin, client;

    for (int i = 0; i < 2; ++i) { //Создаем дочерние процессы - обработчики сообщений

        if (pid) {

            pid = fork();
            sleep(2); //Для неоднородности работы обрабатывающих сообщения процессов

        }

    }

    if (!pid) { //если мы - дочерний процесс

        for(;;) {

            srand(time(NULL));

            if (done == FALSE) { //Если очередь формируется

                while (done != TRUE) {

                    sleep(rand() % 4 + 1);

                }

            }

            for (int i = 0; i < QU_SIZE(); i++) {

                sleep(rand() % 8 + 1);

                if (strcmp(qu[i], "\0") != 0) {

                    printf("CHILD OUT: MESSAGE: %d)\t %s\n", i + 1, qu[i]);
                    snprintf(qu[i], sizeof(qu[i]), "\0");


                } else {

                    ;;

                }          

            }

        }

    } else { //Если мы - родитель (основной процесс)

        serv_sock = Socket(AF_INET, SOCK_STREAM, 0); //Функции - обертки (название начинается с заглавной буквы) искать в unp.h

        if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            perror("setsockopt(SO_REUSEADDR) failed:\t");

        memset(&sin, 0, sizeof(sin));

        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = SERVER_PORT();

        Bind(serv_sock, (struct sockaddr *)&sin, sizeof(sin));

        printf("server started\n");

        Listen(serv_sock, 3);

        for(;;) {

            /*Добавляем прослушиваемый сокет к набору, чтоб фиксировать наличие соединения*/
            FD_ZERO(&fd); 
            FD_SET(serv_sock, &fd);

            Select(serv_sock + 1, &fd, NULL, NULL, NULL);

            if (FD_ISSET(serv_sock, &fd)){ //Если пришел запрос на соединение

                client_sock = accept(serv_sock, (struct sockaddr *)NULL, NULL);
                count++;

                printf("connection #%d open\n", count);

                /*Начало приема сообщений
                Для обозначения конца передачи используется ключевое слово "HALTU"*/
                for(;;) {

                    *done = FALSE;

                    //Добавляем сокет к набору, чтоб проверять налчие данных
                    FD_ZERO(&fd); 
                    FD_SET(client_sock, &fd);
                
                    Select(client_sock + 1, &fd, NULL, NULL, NULL);

                    if (FD_ISSET(client_sock, &fd)) { //Если пришли данные с сокета клиента

                        answ_len = recv(client_sock, buffer, BUF_SIZE(), 0); 

                        strcpy(qu[qu_pos], buffer);
                        snprintf(qu[qu_pos], BUF_SIZE(), "%s", buffer);

                        if (qu_pos < 24) {

                            qu_pos++;

                        } else {

                            qu_pos = 0;

                        }

                    }

                    if (strcmp(buffer, "HALTU") == 0) {

                        break;

                    }

                }

            }

            sleep(2);
            send(client_sock, "HALTU", sizeof("HALTU"), 0); //отправляем клиенту сообщение для завершения

            printf("connection #%d closed\n", count);

            *done = TRUE;

            /*Завершение соединения с текущм клиентом*/
            shutdown(client_sock, SHUT_RDWR);
            close(client_sock);

        }

        return 0;

    }

}

