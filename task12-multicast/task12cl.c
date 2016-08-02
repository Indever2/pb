/* * * * * * * * * * * * * * * * * * * * * * * *
*                                              *
*          Задание 12. Клиент.                 *
*          Multicast-вещание.                  * 
*                                              *
* * * * * * * * * * * * * * * * * * * * * * * */
#include "unp.h"

int main(int argc, char const **argv) {

    int sockfd; // Дескриптор сокета

    char buffer[BUF_SIZE()]; // Буффер сообщения
    struct sockaddr_in s_in; // Параметры сетевого обмена 
    struct ip_mreq mult;   
    socklen_t len = sizeof(struct sockaddr_in); // Длина

    sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Сокет инициализируется как простой UDP-сокет

    memset(&s_in, 0, sizeof(s_in)); 

/* Заполнение параметров */
    s_in.sin_family = AF_INET;
    s_in.sin_addr.s_addr = inet_addr("224.0.0.1");
    s_in.sin_port = htons(CLIENT_PORT());

    mult.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
    mult.imr_interface.s_addr = inet_addr("192.168.1.2");

/* Установка параметров */
    Setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mult, sizeof(mult));

/* Привязка параметров к сокету */
    Bind(sockfd, (struct sockaddr *)&s_in, sizeof(s_in));

/* Прием и вывод сообщения */
   	Recvfrom(sockfd, buffer, BUF_SIZE(), 0, (struct sockaddr *)&s_in, &len);
    
    printf("MESSAGE:\t%s\n", buffer);
/* Закрываем сокет и освобождаем память */
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    return 0;

}