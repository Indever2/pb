/* * * * * * * * * * * * * * * * * * * * * * * *
*     Задание 6.Клиент.                        *
*     Формирование собственных заголовков      * 
*     пакетов с помощью RAW-сокетов.           *
* * * * * * * * * * * * * * * * * * * * * * * */

#include "unp.h"

/* Псево-заголовок для подсчета контрольной суммы UDP */
struct pseudo_header {

    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;

};

/* Функция подсчета контрольной суммы */
u_short csum(u_short *hdr_pt, int nwords) {

    long sum = 0;
    u_short buf = 0;
    
    while (nwords > 1) {

        sum += *hdr_pt++;
        nwords -= sizeof (u_short);

    }

    if (nwords) {

        buf = *(u_char *) hdr_pt << 8;
        sum += ntohs(buf);

    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (u_short)(~sum);

}

/* Функция приема-передачи сообщений и вывода информации о них */
void gv_echo(FILE *f, int sockfd, struct sockaddr *servaddr, socklen_t len, char *buffer) {

    int n = 0; // Длина приянтого сообщения
    char recvline[BUF_SIZE()], *rec2; // Строки для получения сообщений
    struct sockaddr_in *si; // Структура для получения данных о входящем сообщении
    char *recv_addr = malloc(sizeof(char *)* BUF_SIZE()); // Поле адреса (откуда пришло сообщение)

    printf("\n");

    /* Функции обертки искать в unp.h! */
    Sendto(sockfd, buffer, BUF_SIZE(), 0, servaddr, len); 
    
    n = Recvfrom(sockfd, recvline, BUF_SIZE(), 0, servaddr, &len);

    recvline[n] = 0; // Завершающий ноль
 
    rec2 = recvline + 28; // Смещаем указатель к полю DATA принятого пакета

    si = (struct sockaddr_in *)&servaddr; // Приводим servaddr к типу struct sockaddr_in*, для получения нужных нам данных

    recv_addr = inet_ntoa(si->sin_addr);
    printf("N:\t%d\nSIN_ADDR:\t%s\nSIN_FAMILY:\t%d\nSIN_PORT:\t%d\nMESSAGE:%s\n", n, 
                                                recv_addr, si->sin_family, si->sin_port, rec2); // Вывод сообщения и иноформации о нем

}

int main(int argc, char const **argv) {

    int sockfd = 0;
    int one = 1; // для setsockopt
    const int *val = &one; 
    int psize = 0; // Размер псевдо-заголовка

    char buffer[PACKET_LEN()], *data, *pseudogram; // Для формирования собственного пакета.

    struct sockaddr_in servaddr;
    struct pseudo_header psh; // Структура псевдо-заголовка 
    struct iphdr *ip = (struct iphdr *) buffer; // Заголовок IP
    struct udphdr *udp = (struct udphdr *) (buffer + sizeof(struct iphdr)); // Заголовок UDP

    memset(&buffer, 0, PACKET_LEN());
    memset(&servaddr, 0, sizeof(servaddr));

    data = buffer + sizeof(struct iphdr) + sizeof(struct udphdr); // Перемещаем указатель поля DATA 
                                                                  // на конец буфера пакета (после IP и UDP заголовков)
    strcpy(data, "HELLO"); // Заполняем поле DATA сообщением

    /* Данные сервера */
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT());
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_UDP); // Инициализируем RAW - сокет

    /* Заполнение нашего IP-заголовка*/
    ip->check = 0x0000; // Перед подсчетом чексумму необходимо занулить
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 16; 
    ip->id = htons(54321);
    ip->ttl = 64; 
    ip->protocol = IPPROTO_UDP; // UDP
    ip->saddr = inet_addr("127.255.255.12");
    ip->daddr = servaddr.sin_addr.s_addr;

    ip->tot_len = sizeof(struct udphdr) + sizeof(struct iphdr) + strlen(data); // Считаем общую длинну пакета
    ip->check = csum((unsigned short *)buffer, ip->tot_len);  // Считаем контрольную сумму для IP-заголовка

    /* Заполнение нашего UDP-заголовка */
    udp->uh_sport = htons(CLIENT_PORT());
    udp->uh_dport = servaddr.sin_port;
    udp->uh_ulen = htons(sizeof(struct udphdr) + strlen(data));
    udp->uh_sum = 0; // Для UDP-заголовка также зануляем контрольную сумму перед инициализацией

    /* Заполняем псевдо-заголовок, необходимый для подсчета контролной суммы UDP-заголовка */
    psh.source_address = inet_addr("127.255.255.12");
    psh.dest_address = servaddr.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + strlen(data));

    psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
    pseudogram = malloc(psize);

    memcpy(pseudogram, (char *)&psh , sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), udp, sizeof(struct udphdr) + strlen(data));

    udp->uh_sum = csum((unsigned short *)pseudogram, psize); // Считаем контрольную сумму для UDP-заголовка

    Setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)); // Задаем параметры сокета, чтобы сообщить сокетному API, 
                                                                  // что свои пакеты мы формируем сами

    gv_echo(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr), buffer); // Вызывваем функцию передачи-приема сообщения

    /* Отключаем сокет */
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    return 0;

}