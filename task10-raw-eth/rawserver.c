/* * * * * * * * * * * * * * * * * * * * * * * *
*     Задание 10. Сервер.                      *
*     Прием/передача сообщений с RAW-сокета,   * 
*     с нашими собственными заголовками.       *
*     (Ethernet, IP, UDP)                      *
* * * * * * * * * * * * * * * * * * * * * * * */
#include "unp.h"

/* Функция, принимающая сообщений с RAW-сокета и отправляющая его обратно */
void tk_echo(int sockfd, struct sockaddr *client, socklen_t client_len) {

    int n; // Длина принятого сообщения
    socklen_t len; 

    struct sockaddr_in *si; // Структура для получения данных о входящем сообщении

    char buffer[BUF_SIZE()]; // Буффер сообщения

    char *recv_addr = malloc(sizeof(char *)* BUF_SIZE()); // Адрес отправителя

    for (;;) {

        len = client_len;
        n = Recvfrom(sockfd, buffer, BUF_SIZE(), 0, client, &len);

        si = (struct sockaddr_in *)&client; // Приводим servaddr к типу struct sockaddr_in*, для получения нужных нам данных
        recv_addr = inet_ntoa(si->sin_addr); // Получаем адрес отправителя

        /* Выводим сообщение и информацию о нем */
        printf("N:\t%d\nSIN_ADDR:\t%s\nSIN_FAMILY:\t%d\nSIN_PORT:\t%d\nMESSAGE:\t%s\n", n, recv_addr, 
                                                                                    si->sin_family, si->sin_port, buffer);

        sleep(1);

        /* Отправляем ответ */
        Sendto(sockfd, buffer, BUF_SIZE(), 0, client, len);             

    }

}

int main(int argc, char const **argv) {

    int sockfd;
    struct sockaddr_in servaddr, clientaddr;

    sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Сокет инициализируется как простой UDP-сокет

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT());

    Bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    tk_echo(sockfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)); // Функция, принимающая сообщение с RAW-сокета на клиенте и 
                                                                         // отправляющая его обратно
    
/* Закрываем сокет и освобождаем память */
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    return 0;

}