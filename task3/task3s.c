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

    //очередь сообщений релизована через разделяемую память, т.к. обрабатывать ее будут дочерние процессы.
    char **qu = mmap(NULL, sizeof(char **) * QU_SIZE(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
    for (int i = 0; i < QU_SIZE(); i++) {

        qu[i] = mmap(NULL, sizeof(char *) * BUF_SIZE(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        snprintf(qu[i], sizeof(qu[i]), "\0");

    }

    //Завершено ли формирование очереди
    volatile int *done = mmap(NULL, sizeof(int *), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
    *done = FALSE;

    int serv_sock = 0, client_sock = 0;
    int enable = 1;
    int answ_len, count = 0, is_check = 0; 
    int qu_pos = 0; //позиция в очереди

    pid_t pid = 1;

    char buffer[BUF_SIZE()];

    //необходимо для проверки поступления данных на сокет
    fd_set fd;
    struct timeval tv; //для тайм-аута ожидания

    struct sockaddr_in sin, client;

    for (int i = 0; i < 2; ++i) {

        if (pid) {

            pid = fork();
            sleep(1);

        }

    }

    if (!pid) {

        for(;;) {

            srand(time(NULL));

            tv.tv_sec = rand() % 20 + 11; 
            tv.tv_usec = 0;

            printf("STATE: %d\n", *done);

            if (done == FALSE)
                while (done != TRUE) {

                    sleep(1);

                }

            for (int i = 0; i < QU_SIZE(); i++) {

                if (strcmp(qu[i], "\0") != 0) {

                    printf("STATE: %d\n", *done);
                    printf("CHILD OUT: MESSAGE: %d)\t %s\n", i + 1, qu[i]);
                    snprintf(qu[i], sizeof(qu[i]), "\0");

                }

                tv.tv_sec = rand() % 4 + 1; 
                tv.tv_usec = 0;

                select(0, NULL, NULL, NULL, &tv);                

            }

        }

    } else {

        if ( (serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

            perror("SERVER: socket:\t");
            exit(-1);

        }

        if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            perror("setsockopt(SO_REUSEADDR) failed:\t");

        memset(&sin, 0, sizeof(sin));

        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = SERVER_PORT();

        if (bind(serv_sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {

            perror("SERVER: bind:\t");
            exit(-1);

        }

        printf("server started\n");

        if (listen(serv_sock, 3) < 0){

            perror("SERVER: listen:\t");
            exit(3);

        }

        for(;;) {

            FD_ZERO(&fd);
            FD_SET(serv_sock, &fd);

            if (select(serv_sock + 1, &fd, NULL, NULL, NULL) <= 0) {

                perror("SERVER: select:\t");
                exit(3);

            }

            if (FD_ISSET(serv_sock, &fd)){

                client_sock = accept(serv_sock, (struct sockaddr *)NULL, NULL);
                count++;

                printf("connection #%d open\n", count);

                //Для обозначения конца передачи используется ключевое слово "HALTU"
                for(;;) {

                    printf("SERVER STATE: %d\n", *done);

                    *done = FALSE;

                    //Добавляем сокет к набору, чтоб проверять налчие данных
                    FD_ZERO(&fd); 
                    FD_SET(client_sock, &fd);
                
                    select(client_sock + 1, &fd, NULL, NULL, NULL);

                    if (FD_ISSET(client_sock, &fd)) {

                        answ_len = recv(client_sock, buffer, BUF_SIZE(), 0);
                        printf("TAKEN: %s-------BYTES: %d-------POS: %d\n", buffer, answ_len, qu_pos);
                        strcpy(qu[qu_pos], buffer);
                        snprintf(qu[qu_pos], BUF_SIZE(), "%s", buffer);
                        if (qu_pos < 24)
                            qu_pos++;
                        else
                            qu_pos = 0;

                    }

                    if (strcmp(buffer, "HALTU") == 0) {

                        *done = TRUE;
                        printf("SERVER STATE: %d\n", *done);
                        break;

                    }

                }

            }

            sleep(2);
            send(client_sock, "HALTU", sizeof("HALTU"), 0); //отправляем клиенту сообщение для завершения.

            printf("connection #%d closed\n", count);

            printf("MESSEGES ACCEPTED\n=========================\n");

            for (int i = 0; i < QU_SIZE(); i++) {

                if (strcmp(qu[i], "\0") != 0) {

                    printf("%d)\t%s\n", i + 1, qu[i]);

                }

            }

            shutdown(client_sock, SHUT_RDWR);
            close(client_sock);

        }

        return 0;

    }

}

