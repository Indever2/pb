/* * * * * * * * * * * * * * * * * * * * * * * *
*                                              *
*          Задание 11. Сервер.                 *
*          Multicast-вещание.                  * 
*                                              *
* * * * * * * * * * * * * * * * * * * * * * * */
#include "unp.h"

int main(int argc, char const **argv) {

    int sockfd; // Дескриптор сокета
    const int val = 1; // Для установки параметров сокета
    char buffer[BUF_SIZE()]; // Буффер сообщения
    struct sockaddr_in servaddr, clientaddr; // Параметры сервера и клиента

    sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Сокет инициализируется как простой UDP-сокет

    memset(&servaddr, 0, sizeof(servaddr));

/* Заполнение буффера сообщения */
    strcpy(buffer, "HEY ITS MY MULTICAST");

/* Установка параметров сервера и клиента */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT());

    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = inet_addr("224.0.0.1"); // multicast
    clientaddr.sin_port = htons(CLIENT_PORT());

/* Привязка параметров сервера к сокету */
    Bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

/* Отправка broadcast - сообщения */
    Sendto(sockfd, buffer, BUF_SIZE(), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
    
/* Закрываем сокет и освобождаем память */
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    return 0;

}