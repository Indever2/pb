/* * * * * * * * * * * * * * * * * * * * * * * *
*     Задание 6.Сервер.                        *
*     Формирование собственных заголовков      * 
*     пакетов с помощью RAW-сокетов.           *
* * * * * * * * * * * * * * * * * * * * * * * */
#include "unp.h"

int main() {

    int serv_sock = 0, client_sock = 0; //сокеты
    int enable = 1; //Для установки параметров сокета
    int answ_len, count = 0;

    char buffer[BUF_SIZE()]; 

    /*необходимо для проверки поступления данных на сокет*/
    fd_set fd; 
    struct timeval tv; //для тайм-аута ожидания

    struct sockaddr_in sin, client;

    serv_sock = Socket(AF_INET, SOCK_DGRAM, 0); //Функции - обертки (название начинается с заглавной буквы) искать в unp.h

    Setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    memset(&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = SERVER_PORT();

    Bind(serv_sock, (struct sockaddr *)&sin, sizeof(sin));

    printf("server started\n");

    Listen(serv_sock, 3);

    for (;;) {

        FD_ZERO(&fd); 
        FD_SET(serv_sock, &fd);

        Select(serv_sock + 1, &fd, NULL, NULL, NULL);

        if (FD_ISSET(serv_sock, &fd)){ //Если пришел запрос на соединение

            client_sock = accept(serv_sock, (struct sockaddr *)NULL, NULL);
            count++;

            printf("connection #%d open\n", count);

            //Добавляем сокет к набору, чтоб проверять налчие данных
            FD_ZERO(&fd); 
            FD_SET(client_sock, &fd);
            
            Select(client_sock + 1, &fd, NULL, NULL, NULL);

            if (FD_ISSET(client_sock, &fd)) { //Если пришли данные с сокета клиента

                answ_len = recv(client_sock, buffer, BUF_SIZE(), 0); 
                printf("Message recerved: %s\n", buffer);

            }

        }

        printf("connection #%d closed\n", count);

        /*Завершение соединения с текущм клиентом*/
        shutdown(client_sock, SHUT_RDWR);
        close(client_sock);

    }

    return 0;

}